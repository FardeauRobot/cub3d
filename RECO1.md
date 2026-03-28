# RECO1 — Code Review: Parsing to Game Launch

> Written as a tutor would — each section explains **what** is wrong,
> **why** it matters, and **how** to fix it (42 norm-compliant).

---

## Table of Contents

1. [The `t_p_structs` pattern — unnecessary indirection](#1-the-t_p_structs-pattern)
2. [`ft_texture_dispatch` — a 50-line `if` chain that should be a loop](#2-ft_texture_dispatch)
3. [`ft_rgb` — tangled logic, norm-breaking, fragile](#3-ft_rgb)
4. [`ft_textures_detect` — redundant with dispatch](#4-ft_textures_detect)
5. [`ft_file_store` — double copy via linked list](#5-ft_file_store)
6. [`ft_textures_complete` — silent about what's missing](#6-ft_textures_complete)
7. [`ft_player_set` — cascading `if` instead of `else if`](#7-ft_player_set)
8. [Return value inconsistency across parsing](#8-return-value-inconsistency)
9. [`defines.h` — mixing prototypes and macros](#9-definesh-mixing-prototypes-and-macros)
10. [Duplicate `t_minimap` in `t_cub`](#10-duplicate-t_minimap-in-t_cub)
11. [`ft_rgb_convert` — `ft_atoi` returns 0 on bad input, not -1](#11-ft_rgb_convert-silent-bug)
12. [Missing validations in parsing](#12-missing-validations)

---

## 1. The `t_p_structs` pattern

**File:** `main.c:18-29`, `structures.h:63-70`

### What's happening

You created a struct `t_p_structs` that holds pointers to every sub-struct
(`p_cub`, `p_map`, `p_player`, etc.), then you embed a *pointer* to that struct
inside `t_map`, `t_player`, and `t_minimap`. The goal: let any child struct
reach any sibling without the caller passing the parent.

### Why it's a problem

- **It's circular.** `t_cub` contains `t_map`, which contains a pointer back to
  `t_p_structs`, which lives inside `t_cub`. This creates a dependency web that
  makes the code hard to reason about.
- **It hides data flow.** When you read a function signature like
  `ft_minimap_init(t_map *map)`, you'd expect it only needs map data. But
  through `map->p_structs->p_cub`, it can secretly access *everything*. This
  defeats the purpose of separating concerns.
- **It's not actually used much.** Most of your functions already receive
  `t_cub *data` directly. The `p_structs` pointers are mostly dead weight.

### The professional approach

Pass exactly what a function needs. If `ft_minimap_init` needs both `t_map` and
`t_cub`, pass both:

```c
void  ft_minimap_init(t_cub *data, t_map *map);
```

This is explicit, norm-friendly, and zero overhead. Remove `t_p_structs`
entirely, and remove the `p_structs` field from `t_map`, `t_player`, and
`t_minimap`.

**Key lesson:** Good C code makes data dependencies visible in function
signatures. If a function needs parent data, pass the parent — don't build a
global back-pointer system.

---

## 2. `ft_texture_dispatch`

**File:** `parsing_textures.c:105-152`

### What's happening

A 50-line function with 6 independent `if` blocks, each doing almost the same
thing: compare `id`, then store a value. The wall texture blocks are copy-paste
with only the field name changing. The color blocks duplicate each other too.

### Why it's a problem

- **Norm violation risk.** The function is already at ~47 lines — one small
  addition and you'll exceed 25 statements or the line limit.
- **Repetition.** The 4 wall-texture blocks are identical except for one field.
  Copy-pasted logic means copy-pasted bugs.
- **No `else if`.** Every condition is checked even after one already matched.
  Wastes cycles and risks double-assignment if identifiers overlap (and "F" *does*
  match inside "FA" via `ft_strncmp` with n=1).

### How to fix it (norm-compliant)

Use a lookup table for wall textures, and a small helper for colors:

```c
static char  **ft_get_wall_field(t_textures *tex, char *id)
{
    if (ft_strncmp(id, "NO", 3) == 0)
        return (&tex->north);
    if (ft_strncmp(id, "SO", 3) == 0)
        return (&tex->south);
    if (ft_strncmp(id, "WE", 3) == 0)
        return (&tex->west);
    if (ft_strncmp(id, "EA", 3) == 0)
        return (&tex->east);
    return (NULL);
}
```

Then dispatch becomes:

```c
static int  ft_texture_dispatch(t_cub *data, char **arr, char *id, char *path)
{
    char  **field;

    field = ft_get_wall_field(&data->textures, id);
    if (field)
        return (ft_assign_wall(data, field, path));
    if (ft_strncmp(id, "F", 2) == 0)
        return (ft_assign_color(data, arr, FLOOR));
    if (ft_strncmp(id, "C", 2) == 0)
        return (ft_assign_color(data, arr, CEILING));
    return (FAILURE);
}
```

~15 lines instead of 50. Each helper is small, testable, and norm-safe.

**Key lesson:** When you see 4+ blocks of near-identical code, extract the
varying part (here: the field pointer) and write one generic block.

---

## 3. `ft_rgb`

**File:** `parsing_textures.c:38-82`

### What's happening

You iterate over `arr` (the split line), then *inside* that loop, you split
again by `,`, then manually assign `rgb[0]`, `rgb[1]`, `rgb[2]` with three
cascading `if` blocks. You also increment `j` manually inside the `while`
loop that already increments `j`.

### Why it's a problem

- **Double `j++`.** The `while (split[++j])` loop increments `j`, then you
  also do `j++` inside the body after assigning rgb[0] and rgb[1]. This means
  you're skipping elements. It works *by accident* when each `arr[i]` has
  exactly the right tokens, but it's extremely fragile.
- **Norm violation.** This function has too many variables and too many nested
  levels. You noted the TODO yourself at line 36.
- **Over-engineering.** You split by whitespace (getting `arr`), then split
  each element by `,`. But the line format is `F 220,100,0` — there's only
  *one* token after the identifier. A single split by `,` on `arr[1]` is enough.

### How to fix it

```c
static int  ft_rgb(t_cub *data, char *color_str, t_background part)
{
    char  **split;
    int   rgb[3];
    int   i;

    split = ft_split_charset_gc(color_str, ",", &data->gc_tmp);
    if (!split || ft_arrlen(split) != 3)
        return (ft_error(ERR_MSG_INVALID_RGB, color_str, ERRN_PARSING));
    i = -1;
    while (++i < 3)
    {
        if (!ft_is_only(split[i], ft_isdigit) || !split[i][0])
            return (ft_error(ERR_MSG_INVALID_RGB, split[i], ERRN_PARSING));
        rgb[i] = ft_atoi(split[i]);
        if (rgb[i] < 0 || rgb[i] > 255)
            return (ft_error(ERR_MSG_INVALID_RGB, split[i], ERRN_PARSING));
    }
    ft_rgb_affect(data, rgb, part);
    return (SUCCESS);
}
```

Pass `arr[1]` directly from the caller instead of passing the whole `arr`.
This eliminates the double loop, the manual `j++`, and `ft_rgb_convert`
(which becomes a single line inside the loop).

**Key lesson:** When parsing a known format, match your code to the format's
structure. `R,G,B` is 3 tokens separated by commas — one split, one loop of 3.

---

## 4. `ft_textures_detect`

**File:** `parsing_textures.c:88-98`

### What's happening

This function checks whether a string is one of `NO SO EA WE F C`. It's called
in `ft_textures_fill` before `ft_texture_dispatch`.

### Why it's useless

`ft_texture_dispatch` already does the exact same comparisons. If the ID
doesn't match anything, dispatch can return `FAILURE` directly. You're
comparing every string *twice*.

### How to fix it

Delete `ft_textures_detect`. In `ft_textures_fill`, just call
`ft_texture_dispatch` directly and handle its return value.

**Key lesson:** Before writing a validation function, check if the operation
itself already validates. "Try and report failure" is cleaner than "check then
try."

---

## 5. `ft_file_store`

**File:** `parsing_utils.c:67-98`

### What's happening

You read lines into a linked list (with GC), then allocate a `char **` array,
then `ft_strdup_gc` every line from the list into the array. This means every
line exists in memory *twice*: once in `gc_tmp` (from GNL) and once in
`gc_global` (from strdup).

### Why it's a problem

- **Wasted memory.** Every line is duplicated.
- **Wasted CPU.** You're doing `strlen + memcpy` for every line just to move
  it from one allocator to another.
- **The linked list is unnecessary.** You only use it to count lines and then
  iterate sequentially — a dynamic array (realloc pattern) does the same in
  one pass.

### How to fix it

Option A (simplest): Count lines first, then read again. But GNL doesn't
support seeking back.

Option B (recommended): Use a growing array. Start with 16 slots, double when
full. This is a standard pattern:

```c
int  ft_file_store(t_cub *data)
{
    char  *line;
    int   i;
    int   capacity;

    capacity = 16;
    data->file = ft_calloc_gc(capacity, sizeof(char *), &data->gc_global);
    if (!data->file)
        return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
    i = 0;
    line = get_next_line(data->fd_map);
    while (line)
    {
        if (i >= capacity - 1)
            // reallocate with doubled capacity (you'll need a gc-aware realloc)
        data->file[i++] = line;  // store directly, no strdup
        line = get_next_line(data->fd_map);
    }
    data->file[i] = NULL;
    return (SUCCESS);
}
```

If you don't want to implement gc-aware realloc, at minimum stop doing
`ft_strdup_gc` — just move the GNL pointers into the array and track them
in `gc_global` directly.

**Key lesson:** linked list -> array conversion is almost always a sign you
should have used an array from the start.

---

## 6. `ft_textures_complete`

**File:** `parsing_textures.c:176-185`

### What's happening

Returns `FAILURE` if any texture is missing, but doesn't say *which* one.

### Why it matters

When a user forgets the `EA` line in their `.cub` file, they get:
`"Error while loading game: Couldn't load all textures"`. They have no idea
which one is missing.

### How to fix it

```c
int  ft_textures_complete(t_textures *tex)
{
    if (!tex->north)
        return (ft_error(ERR_MSG_TEXTURES, "NO", ERRN_TEXTURES));
    if (!tex->south)
        return (ft_error(ERR_MSG_TEXTURES, "SO", ERRN_TEXTURES));
    if (!tex->east)
        return (ft_error(ERR_MSG_TEXTURES, "EA", ERRN_TEXTURES));
    if (!tex->west)
        return (ft_error(ERR_MSG_TEXTURES, "WE", ERRN_TEXTURES));
    if (tex->ceiling_rgb == -1)
        return (ft_error(ERR_MSG_TEXTURES, "C", ERRN_TEXTURES));
    if (tex->floor_rgb == -1)
        return (ft_error(ERR_MSG_TEXTURES, "F", ERRN_TEXTURES));
    return (SUCCESS);
}
```

**Key lesson:** Error messages should tell the user *exactly* what to fix.
Generic errors waste everyone's time.

---

## 7. `ft_player_set`

**File:** `parsing_utils.c:22-50`

### What's happening

Four `if` blocks that are mutually exclusive but not written as `else if`.

### Why it matters

- All 4 conditions are tested every time, even though only one can match.
- It signals to a reader: "maybe multiple could match?" — which is misleading.

### How to fix it

```c
void  ft_player_set(t_player *player, int x, int y, char orient)
{
    player->pos_y = (double)y + 0.5;
    player->pos_x = (double)x + 0.5;
    if (orient == 'N')
        ft_set_dir(player, 0, -1, NORTH);
    else if (orient == 'S')
        ft_set_dir(player, 0, 1, SOUTH);
    else if (orient == 'E')
        ft_set_dir(player, 1, 0, EAST);
    else if (orient == 'W')
        ft_set_dir(player, -1, 0, WEST);
}
```

Where `ft_set_dir` is a 5-line helper that assigns `dir_x`, `dir_y`, and
`orient`. This also reduces `ft_player_set` to well under the norm limit.

**Key lesson:** Use `else if` when conditions are mutually exclusive. It's
both faster and communicates intent.

---

## 8. Return value inconsistency

### What's happening

Some functions return `int` error codes (`ft_parsing`, `ft_textures_parsing`),
but the caller ignores them and calls `ft_exit` anyway. Other functions return
`void` and call `ft_exit` internally (`ft_map_check`, `ft_map_fill`).

**Example from `parsing.c`:**
```c
if (ft_textures_parsing(data) != SUCCESS)       // returns int
    ft_exit(data, ERRN_PARSING, NULL, NULL);     // but exit is called here

ft_map_fill(data);                                // void, exits internally
ft_map_check(data);                               // void, exits internally
```

### Why it matters

A reader can't predict the error strategy by looking at a function signature.
Will it return an error? Will it exit? Both? Pick one pattern and stick to it.

### Recommended pattern

**For parsing:** have every function return `int`. The caller decides whether
to exit. This makes functions reusable and testable:

```c
if (ft_map_fill(data) != SUCCESS)
    ft_exit(data, ERRN_PARSING, ERR_MSG_PARSING, ERR_MSG_MALLOC);
if (ft_map_check(data) != SUCCESS)
    ft_exit(data, ERRN_PARSING, NULL, NULL);
```

**Key lesson:** Consistent error handling patterns are a hallmark of
professional C code. "Return errors, let the caller decide" is the standard.

---

## 9. `defines.h` — mixing prototypes and macros

**File:** `defines.h`

### What's happening

This file contains `#define` macros **and** all function prototypes. It also
`#include`s `structures.h`, and `structures.h` `#include`s `defines.h` —
creating a circular dependency (which only works because of include guards).

### Why it matters

- Function prototypes belong in a header that describes the module's API
  (e.g., `cub3d.h` or per-module headers).
- Circular includes are a code smell — one header should depend on the other,
  not both on each other.

### How to fix it

- Move all function prototypes into `cub3d.h` (or create `prototypes.h`).
- Keep `defines.h` for macros only.
- Have `structures.h` *not* include `defines.h`. Instead, include both from
  `cub3d.h` in the right order.

---

## 10. Duplicate `t_minimap` in `t_cub`

**File:** `structures.h:162`

### What's happening

`t_cub` has both `t_map map` (which contains `t_minimap minimap`) and a
separate `t_minimap minimap` field at the top level. In `error.c:44`, you
access `data->minimap.cache.img` but also `data->map.minimap` elsewhere.

### Why it matters

Two minimaps. One is probably unused or out of sync. This *will* cause bugs.

### How to fix it

Remove `t_minimap minimap` from `t_cub`. Access it only through
`data->map.minimap`. Grep for `data->minimap` and replace with
`data->map.minimap`.

---

## 11. `ft_rgb_convert` — silent bug

**File:** `parsing_textures.c:16-22`

### What's happening

```c
*nb = ft_atoi(str);
if (*nb == -1 || *nb > 255)
```

`ft_atoi` returns `0` for non-numeric strings, not `-1`. So:
- `ft_atoi("abc")` returns `0` — passes your check (0 is valid RGB).
- `ft_atoi("-1")` returns `-1` — fails, but for the wrong reason.

The `ft_is_only(split[j], ft_isdigit)` check in `ft_rgb` catches non-digits
*before* this function is called, so the bug doesn't trigger **today**. But
it's a trap for future changes.

### How to fix it

Since you already validate digits upstream, `ft_rgb_convert` should only check
the range:

```c
*nb = ft_atoi(str);
if (*nb < 0 || *nb > 255)
    return (ft_error(ERR_MSG_INVALID_RGB, str, ERRN_PARSING));
```

Or better: remove `ft_rgb_convert` entirely and inline the check in the
simplified `ft_rgb` (see recommendation #3).

**Key lesson:** Know exactly what your standard library functions return.
`atoi("abc") == 0` is a classic C trap.

---

## 12. Missing validations

### 12a. Duplicate texture identifiers

If the `.cub` file contains `NO` twice, the second silently overwrites the
first (leaking the old GC-allocated string). Add a check:

```c
if (*field != NULL)
    return (ft_error("Duplicate texture", id, ERRN_TEXTURES));
```

### 12b. No player on the map

`ft_map_check` checks for *too many* players but never checks for *zero*
players. After the loop, add:

```c
if (data->player.pos_x == 0 && data->player.pos_y == 0)
    ft_exit(data, ERRN_PARSING, ERR_MSG_PARSING, "No player on the map");
```

### 12c. `ft_strncmp` with `n=1` for "F" and "C"

```c
ft_strncmp(id, "F", 1)
```

This matches "F" but also "FOO", "FLOOR", etc. Use `n=2` to include the
null terminator, or compare with `ft_strcmp` / check length.

---

## Summary — Priority Order

| Priority | Issue | Impact | Effort |
|----------|-------|--------|--------|
| 1 | Fix `ft_rgb` (rec #3) | Bug risk + norm | Medium |
| 2 | Fix `ft_strncmp` n=1 for F/C (rec #12c) | Actual bug | Tiny |
| 3 | Remove duplicate minimap (rec #10) | Actual bug | Tiny |
| 4 | Refactor `ft_texture_dispatch` (rec #2) | Norm + clarity | Medium |
| 5 | Delete `ft_textures_detect` (rec #4) | Dead code | Tiny |
| 6 | Add missing validations (rec #12) | Robustness | Small |
| 7 | Consistent error returns (rec #8) | Code quality | Medium |
| 8 | Remove `t_p_structs` (rec #1) | Architecture | Large |
| 9 | Simplify `ft_file_store` (rec #5) | Performance | Medium |
| 10 | Separate prototypes from defines (rec #9) | Hygiene | Small |

Start from the top — fix bugs first, then simplify, then restructure.

---
---

# Part 2 — Becoming a Better Developer

> Based on what I observed across your entire cub3d codebase: your strengths,
> your recurring weaknesses, and the habits that separate someone who writes
> code from someone who writes *beautiful* code.

---

## Your Strengths (keep doing these)

### 1. You structure your project well from the start

Your directory layout (`src/core/`, `src/parsing/`, `src/render/`, `src/utils/`,
`includes/`) is clean and logical. Many 42 students dump everything into one
folder. You didn't. This shows you *think* about organization before coding,
and that instinct will serve you for your entire career.

### 2. You use a garbage collector

Building `gc_global` and `gc_tmp` with separate lifetimes is a real engineering
decision. Most students either leak memory or spend hours chasing `free()`
calls. You built a system. It's not perfect yet (the `gc_tmp`/`gc_global`
boundary is sometimes blurry), but the *thinking* behind it is strong.

### 3. You separate parsing from execution

Your pipeline is clear: parse everything, validate, *then* launch the game.
No mixing of file I/O with rendering. This is the correct architecture, and
it means bugs in one phase don't contaminate the other.

### 4. You write comments that explain *why*, not *what*

Most of your comments describe the purpose of a function, not what each line
does. That's the right instinct. `// CHECKS FOR EMPTY CELLS OR NO WALLS` is
infinitely better than `// increment i` on a `i++` line.

---

## Your Weaknesses (and how to fix them)

### W1. You code before you model the data

**What I observed:** `t_p_structs` exists because you hit a situation where a
child struct needed parent data — so you built a solution *on top of* the
existing design rather than stepping back and rethinking it.

**The habit to build:** Before writing any function, ask:
> "What data does this function need, and where does that data live?"

If the answer requires navigating 3 struct levels, your data model is wrong.
Redesign the structs *first*. The best developers spend more time thinking
about data shapes than writing code.

**Practice:** Next time you're about to add a pointer field to "link" two
structs, stop. Draw the struct hierarchy on paper. Ask: "Can I just pass
this as a parameter instead?" The answer is almost always yes.

### W2. You copy-paste first, refactor never

**What I observed:** `ft_texture_dispatch` has 4 nearly identical blocks for
NO/SO/EA/WE. `ft_char_draw` and `ft_orient_draw` share 90% of their code.
`ft_player_set` repeats the same assignment pattern 4 times.

**Why this happens:** When you're under pressure (or excited to see it work),
copy-paste feels fast. And it *is* fast — for the first version. But every
copy creates a maintenance liability. Change one, forget the other, get a bug.

**The habit to build:** Follow the **Rule of Three**:
1. First time: just write it.
2. Second time: copy it, feel a little guilty.
3. Third time: **stop and extract a function.**

After pasting something for the third time, that's your signal. The varying
parts become parameters, the identical parts become the function body.

### W3. You don't test your edge cases mentally

**What I observed:**
- `ft_strncmp(id, "F", 1)` — you didn't ask "what if the string is `FLOOR`?"
- `ft_atoi` returning 0 for bad input — you didn't ask "what does atoi actually
  return on failure?"
- No check for zero players — you checked for *too many* but didn't ask "what
  if there are none?"

**The habit to build:** After writing a condition, spend 30 seconds asking:
> "What are the 3 worst inputs someone could give this function?"

For a string comparison: empty string, longer string, similar prefix.
For a number: 0, -1, MAX_INT.
For an array: empty, one element, NULL.

This is called **boundary thinking**, and it's the single most impactful
habit a developer can build. Bugs almost always live at boundaries.

### W4. You mix languages and conventions

**What I observed:**
- Comments in French *and* English in the same file
- `ft_` prefix used everywhere (42 convention), but comments explaining it
  are inconsistent in style
- Some functions have block comments (`/* */`), others have `//` one-liners,
  others have both, others have none

**The habit to build:** Pick one language, one comment style, and stick to it
across the *entire* project. It doesn't matter which — what matters is
consistency. A codebase that looks like one person wrote it is easier to
read than one that looks like a committee.

**Practical rule:** English for code, comments, and commit messages. French
for conversation. Never mix them in the same file.

---

## Habits of Developers Who Write Beautiful Code

### H1. Read code more than you write it

You will learn more by reading 100 lines of good code than by writing 1000
lines of mediocre code. Find 42 students who got outstanding scores on
cub3d or minishell and read their code. Read the source of `libft` functions
you didn't write. When you see a pattern you like, steal it.

**Concrete action:** Every week, spend 30 minutes reading someone else's
project. Not skimming — *reading*. Understand why they made each decision.

### H2. Make the compiler your ally, not your enemy

You compile with `-Wall -Wextra -Werror` (good). But don't just fix warnings
to make them go away — understand what the compiler is telling you. Every
warning is the compiler saying: "I think you might have a bug here."

Also: when your code compiles but does the wrong thing, the bug is almost
always in a place you were *certain* was correct. Question your certainty.

### H3. Name things so a stranger can read your code

Your naming is generally good (`ft_textures_parsing`, `ft_map_check`), but
some names are vague:

- `arr` — array of *what*? Call it `tokens` or `parts`.
- `ret` — return value of *what*? Call it `color` or `err`.
- `data` — this is fine for the root struct, but when you also call local
  variables `data`, it shadows and confuses.

**The rule:** If you need a comment to explain a variable, the variable has
the wrong name. Rename it, and delete the comment.

### H4. Write small functions with one job

The 42 norm forces this (25 lines max), but don't think of it as a
constraint — it's a *gift*. Functions longer than 15 lines are almost always
doing too many things. Beautiful code reads like a story:

```c
int  ft_parsing(t_cub *data, char **argv, int argc)
{
    ft_validate_args(data, argv, argc);
    ft_open_file(data, argv[1]);
    ft_store_file(data);
    ft_parse_textures(data);
    ft_build_map(data);
    ft_validate_map(data);
    ft_gc_free_all(&data->gc_tmp);
    return (SUCCESS);
}
```

Each line is a chapter. You can understand the whole function without reading
any of the helpers. That's beautiful code.

### H5. Delete code you don't need

**What I observed:** `ft_textures_detect` duplicates logic. `vectors.c` is
empty. `t_p_structs` was unused machinery. The commented-out FPS counter in
`render_map.c` has been there for weeks.

Dead code is noise. It makes the codebase feel heavier than it is, and it
distracts you when searching. Delete it. If you need it later, that's what
`git log` is for.

**The rule:** If it's commented out, delete it. If it's unused, delete it. If
it might be useful someday, delete it — and use git to bring it back *someday*.

### H6. Commit often, with intention

Your recent commit messages are good (`On a fini la gestion des couleurs`),
but commit *more often* and in English. A good commit should be one logical
change:

- "Add RGB color parsing for F and C identifiers"
- "Fix wall closure check for edge-of-map tiles"
- "Remove unused t_p_structs indirection"

Not: "On continue le parsing" (too vague — parsing of *what*?).

Small, well-described commits make `git bisect` possible when you hunt bugs.
They also force you to think in small, complete steps rather than big chaotic
sessions.

### H7. Understand your tools deeply

**What I observed:** You use `ft_strncmp` but didn't fully think through
what `n` means (it compares *at most* n characters — so `n=1` on `"F"` will
match any string starting with F). You use `ft_atoi` but assumed it returns
`-1` on error.

For every function in your libft: know its *exact* behavior on edge cases.
Write a tiny test:

```c
printf("%d\n", ft_atoi(""));      // what does this return?
printf("%d\n", ft_atoi("abc"));   // and this?
printf("%d\n", ft_atoi("-0"));    // and this?
```

Run it. Memorize the answers. This takes 5 minutes and saves hours of
debugging.

---

## Summary: Your Growth Roadmap

```
WHERE YOU ARE NOW
  |
  |  You build structured projects with clear separation.
  |  You think about memory management.
  |  You make things work.
  |
  v
NEXT LEVEL: Eliminate repetition (Rule of Three)
  |
  |  - Extract helpers when you see copy-paste
  |  - Use lookup tables instead of if/if/if/if
  |  - Make every function do exactly one thing
  |
  v
NEXT LEVEL: Think at the boundaries
  |
  |  - Test edge cases before you ship
  |  - Know your tools' exact behavior
  |  - Question every assumption
  |
  v
NEXT LEVEL: Write code for the reader
  |
  |  - Name things precisely
  |  - Delete dead code ruthlessly
  |  - Commit small, describe well
  |
  v
THE GOAL: Code that doesn't need comments because it reads like prose.
```

You're closer than you think. The fact that you *asked* for this review puts
you ahead of most students. Keep that curiosity — it's your best tool.
