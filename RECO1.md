# RECO1 — Code Review: Parsing to Game Launch (Updated 2026-03-30)

> Written as a tutor would — each section explains **what** is wrong,
> **why** it matters, and **how** to fix it (42 norm-compliant).
>
> Issues marked **[FIXED]** have been resolved since the first review.
> Issues marked **[NEW]** are regressions or newly introduced problems.

---

## Table of Contents

### Fixed Issues
- [1. The `t_p_structs` pattern — FIXED](#1-the-t_p_structs-pattern--fixed)
- [11. `ft_rgb_convert` silent bug — FIXED](#11-ft_rgb_convert-silent-bug--fixed)
- [12b. No player on the map — FIXED](#12b-no-player-on-the-map--fixed)

### Still Open Issues
- [2. `ft_texture_dispatch` — a 50-line `if` chain that should be a loop](#2-ft_texture_dispatch)
- [3. `ft_rgb` — tangled logic, norm-breaking, fragile](#3-ft_rgb)
- [4. `ft_textures_detect` — redundant with dispatch](#4-ft_textures_detect)
- [5. `ft_file_store` — double copy via linked list](#5-ft_file_store)
- [6. `ft_textures_complete` — silent about what's missing](#6-ft_textures_complete)
- [7. `ft_player_set` — cascading `if` instead of `else if`](#7-ft_player_set)
- [8. Return value inconsistency across parsing](#8-return-value-inconsistency)
- [9. `defines.h` — mixing prototypes and macros](#9-definesh-mixing-prototypes-and-macros)
- [10. Duplicate `t_minimap` in `t_cub`](#10-duplicate-t_minimap-in-t_cub)
- [12a. Duplicate texture identifiers — partially fixed](#12a-duplicate-texture-identifiers)
- [12c. `ft_strncmp` with `n=1` for "F" and "C"](#12c-ft_strncmp-with-n1-for-f-and-c)

### New Issues
- [N1. Debug `printf` left in `ft_rgb_convert`](#n1-debug-printf-left-in-ft_rgb_convert)
- [N2. `ft_atoi_safe` doesn't initialize `*nb` itself](#n2-ft_atoi_safe-doesnt-initialize-nb-itself)
- [N3. Misplaced comment block above `ft_player_set`](#n3-misplaced-comment-block-above-ft_player_set)
- [N4. `ft_rgb_affect` uses `if/if` instead of `if/else if`](#n4-ft_rgb_affect-uses-ifif-instead-of-ifelse-if)
- [N5. `ft_textures_parsing` returns SUCCESS on incomplete file](#n5-ft_textures_parsing-returns-success-on-incomplete-file)

---

## Fixed Issues

### 1. The `t_p_structs` pattern — [FIXED]

**Status:** Fully removed. No traces remain in the codebase. Functions now
receive exactly the data they need via their parameters. Well done — this
was the biggest architectural problem and you solved it cleanly.

---

### 11. `ft_rgb_convert` silent bug — [FIXED]

**File:** `parsing_textures.c:15-28`

**What you did:** You replaced the raw `ft_atoi` call with a new `ft_atoi_safe`
function that validates digits before converting. This eliminates the original
bug where `ft_atoi("abc")` returned 0 (a valid RGB value) and passed silently.

Good fix. The new `ft_atoi_safe` checks `ft_is_only(str, ft_isdigit)` and
caps string length at 5 chars — both sensible guards.

---

### 12b. No player on the map — [FIXED]

**File:** `parsing.c:133-134`

**What you did:** Added a check after `ft_map_check`:
```c
if (data->player.pos_x == 0 || data->player.pos_y == 0)
    ft_exit(data, ERRN_PARSING, ERR_MSG_PARSING, ERR_MSG_NO_PLAYER);
```

And defined `ERR_MSG_NO_PLAYER` in `errors.h`. This is exactly the right
approach. The zero-player case is now caught.

---

## Still Open Issues

### 2. `ft_texture_dispatch`

**File:** `parsing_textures.c:117-165`

**Status:** Unchanged since last review.

Still ~48 lines with 6 independent `if` blocks (no `else if`). The 4 wall
texture blocks are copy-pasted with only the field name changing. Every
condition is tested even after one already matched.

**Why it matters now more than before:** You *did* add duplicate-color checks
(lines 146, 156), which is good — but it made the function even longer. This
function is dangerously close to the 25-statement norm limit and will break
the norm as soon as you add anything else.

**The fix is the same as before:** extract `ft_get_wall_field` and use
`else if` chains. See the original recommendation for the code.

---

### 3. `ft_rgb`

**File:** `parsing_textures.c:55-99`

**Status:** Unchanged. The TODO comment on line 53 is still there:
`// TODO : A REFACTORISER POUR LA NORME, TROUVER UN MOYEN PLUN FUN`

Still has the double-loop pattern (outer loop on `arr`, inner loop on
comma-split), still has manual `j++` increments inside a `while(split[++j])`
loop, still has 3 cascading `if/else if` blocks for rgb[0], rgb[1], rgb[2].

You know it needs refactoring — you wrote the TODO yourself. The fix from
the original review still applies: pass `arr[1]` directly, split once by
`,`, loop 3 times.

---

### 4. `ft_textures_detect`

**File:** `parsing_textures.c:105-110`

**Status:** Still exists, still redundant.

This function checks `NO SO EA WE F C` — `ft_texture_dispatch` does the
exact same comparisons. You're comparing every identifier twice. Delete it
and let dispatch return FAILURE directly.

**Bonus problem:** `ft_textures_detect` also has the `ft_strncmp(id, "F", 1)`
bug (see #12c). If you delete this function, you remove one instance of the
bug for free.

---

### 5. `ft_file_store`

**File:** `parsing_utils.c:64-95`

**Status:** Unchanged. Still reads into a linked list, then copies every line
via `ft_strdup_gc` into a `char **` array. Every line exists in memory twice.

---

### 6. `ft_textures_complete`

**File:** `parsing_textures.c:188-197`

**Status:** Unchanged. Still returns a bare `FAILURE` without saying which
texture is missing. The user gets `"Couldn't load all textures"` and has to
guess which of the 6 identifiers they forgot.

---

### 7. `ft_player_set`

**File:** `parsing_utils.c:20-48`

**Status:** Unchanged. Still uses 4 independent `if` blocks instead of
`else if`. All 4 conditions are tested even though only one can ever match.

---

### 8. Return value inconsistency

**Status:** Partially improved.

`ft_file_store` and `ft_textures_parsing` now return `int` and the caller
in `ft_parsing` checks their return values — good.

But `ft_map_fill` (parsing.c:35) and `ft_map_check` (parsing.c:70) still
return `void` and call `ft_exit` internally. The inconsistency remains: some
parsing functions return errors, others exit directly.

---

### 9. `defines.h` — mixing prototypes and macros

**File:** `defines.h`

**Status:** Unchanged. Still contains all function prototypes mixed with
`#define` macros. Still has the circular include: `defines.h` includes
`structures.h` (line 16), and `structures.h` includes `defines.h` (line 16).

---

### 10. Duplicate `t_minimap` in `t_cub`

**File:** `structures.h:147-148`

**Status:** Still present. `t_cub` has:
- `t_map map` (line 147) — which contains `t_minimap minimap` (line 118)
- `t_minimap minimap` (line 148) — a separate, duplicate minimap

Two minimaps in the same struct. One is likely unused or out of sync. This
**will** cause bugs when you start rendering the minimap and accidentally
write to one but read from the other.

**Fix:** Remove line 148 (`t_minimap minimap`) from `t_cub`. Access the
minimap only through `data->map.minimap`. Grep for `data->minimap` and
replace with `data->map.minimap`.

---

### 12a. Duplicate texture identifiers

**Status:** Partially fixed.

You added duplicate checks for **colors** (F and C) in `ft_texture_dispatch`:
```c
if (data->textures.floor_rgb != 0)
    return (ft_error(ERR_MSG_SET_COLOR, "FLOOR", ERRN_TEXTURES));
```

But **wall textures** (NO, SO, EA, WE) still have no duplicate check. If the
`.cub` file contains `NO` twice, the second silently overwrites the first.

Add the same pattern for walls:
```c
if (data->textures.north != NULL)
    return (ft_error(ERR_MSG_SET_COLOR, "NO", ERRN_TEXTURES));
```

---

### 12c. `ft_strncmp` with `n=1` for "F" and "C"

**File:** `parsing_textures.c:144, 154` and `parsing_textures.c:107`

**Status:** Still present. Still a bug.

```c
ft_strncmp(id, "F", 1)  // matches "F", "FOO", "FLOOR", anything starting with F
ft_strncmp(id, "C", 1)  // matches "C", "CEILING", "CAT", etc.
```

This appears in both `ft_texture_dispatch` (lines 144, 154) and
`ft_textures_detect` (line 107).

**Fix:** Change `n` from `1` to `2` to include the null terminator:
```c
ft_strncmp(id, "F", 2)  // matches exactly "F" and nothing else
ft_strncmp(id, "C", 2)  // matches exactly "C" and nothing else
```

---

## New Issues

### N1. Debug `printf` left in `ft_rgb_convert`

**File:** `parsing_textures.c:35`

```c
printf("INT = %d\n", *nb);
```

A debug `printf` was left in the error path of `ft_rgb_convert`. This will
print to stdout during normal error handling, polluting the user's terminal.
It also means you're including `<stdio.h>` somewhere, which is unnecessary
for a project that should only use `write` / `ft_printf`.

**Fix:** Delete line 35.

**Key lesson:** Before committing, do a quick `grep -rn "printf" src/` to
catch stray debug prints. Better yet, use a `DEBUG` macro so you can toggle
all debug output at once.

---

### N2. `ft_atoi_safe` doesn't initialize `*nb` itself

**File:** `parsing_textures.c:15-28`

```c
int ft_atoi_safe(char *str, int *nb)
{
    int i;

    i = -1;
    if (ft_strlen(str) > 5 || !ft_is_only(str, ft_isdigit))
    {
        *nb = -1;
        return (FAILURE);
    }
    while (str[++i])
        *nb = *nb * 10 + str[i] - '0';   // uses *nb without initializing it
    return (SUCCESS);
}
```

The function assumes `*nb` is already 0 before the `while` loop. It works
**today** because `ft_rgb_convert` does `*nb = 0` on line 32 before calling.
But `ft_atoi_safe` is a general-purpose function — if anyone else calls it
without pre-zeroing, they'll get garbage.

**Fix:** Add `*nb = 0;` at the start of `ft_atoi_safe`, before the while loop:
```c
*nb = 0;
while (str[++i])
    *nb = *nb * 10 + str[i] - '0';
```

**Key lesson:** A function should never rely on the caller to initialize its
output parameter. The function owns the output — it should set it up.

---

### N3. Misplaced comment block above `ft_player_set`

**File:** `parsing_utils.c:16-19`

```c
/*
** FT_FORMAT_CHECK - VALIDATES THAT THE FILE EXTENSION IS .CUB
** RETURNS SUCCESS IF VALID, FAILURE OTHERWISE
*/
void ft_player_set(t_player *player, int x, int y, char orient)
```

The comment says "FT_FORMAT_CHECK" but the function is `ft_player_set`.
Looks like a copy-paste leftover when functions were reordered.

**Fix:** Either delete the comment or update it to describe `ft_player_set`.

---

### N4. `ft_rgb_affect` uses `if/if` instead of `if/else if`

**File:** `parsing_textures.c:47-50`

```c
if (part == FLOOR)
    data->textures.floor_rgb = ret;
if (part == CEILING)
    data->textures.ceiling_rgb = ret;
```

Same pattern as #7 (`ft_player_set`). FLOOR and CEILING are mutually
exclusive — use `else if`. Minor issue, but it's the same habit repeating.

---

### N5. `ft_textures_parsing` returns SUCCESS on incomplete file

**File:** `parsing_textures.c:203-223`

```c
while (data->file[++i])
{
    ret = ft_textures_fill(data, data->file[i]);
    if (ret != SUCCESS && ret != FAILURE)
        return (ret);
    if (ret == FAILURE && !ft_is_only(data->file[i], ft_isspace))
    {
        if (ft_textures_complete(&data->textures))
            return (ft_error(ERR_MSG_LOADING, ERR_MSG_TEXTURES, ERRN_LOAD));
        data->map.index_map_start = i;
        return (SUCCESS);
    }
}
return (SUCCESS);   // <-- reaches here if file has only texture lines
```

If the `.cub` file contains only texture definitions and no map at all (or
only whitespace lines after textures), the `while` loop finishes without ever
hitting the `FAILURE` branch, and returns `SUCCESS` without setting
`index_map_start`. Then `ft_map_fill` will try to read from index 0 of the
file array, treating texture lines as map data.

**Fix:** After the while loop, check that textures are complete and that a
map was actually found:

```c
if (ft_textures_complete(&data->textures))
    return (ft_error(ERR_MSG_LOADING, ERR_MSG_TEXTURES, ERRN_LOAD));
return (ft_error(ERR_MSG_PARSING, "No map found in file", ERRN_PARSING));
```

---

## Summary — Priority Order (Updated)

| Priority | Issue | Status | Impact | Effort |
|----------|-------|--------|--------|--------|
| 1 | **N1.** Remove debug `printf` | **NEW** | Ships broken output | Tiny |
| 2 | **12c.** Fix `ft_strncmp` n=1 for F/C | Open | Actual bug | Tiny |
| 3 | **10.** Remove duplicate minimap | Open | Actual bug | Tiny |
| 4 | **N2.** Initialize `*nb` in `ft_atoi_safe` | **NEW** | Latent bug | Tiny |
| 5 | **N5.** Handle missing map in texture parsing | **NEW** | Crash risk | Small |
| 6 | **N3.** Fix misplaced comment | **NEW** | Confusing | Tiny |
| 7 | **3.** Refactor `ft_rgb` | Open | Norm + clarity | Medium |
| 8 | **2.** Refactor `ft_texture_dispatch` | Open | Norm + clarity | Medium |
| 9 | **4.** Delete `ft_textures_detect` | Open | Dead code | Tiny |
| 10 | **12a.** Add wall texture duplicate check | Partial | Robustness | Small |
| 11 | **6.** Make `ft_textures_complete` specific | Open | UX | Small |
| 12 | **7.** Use `else if` in `ft_player_set` | Open | Code quality | Tiny |
| 13 | **N4.** Use `else if` in `ft_rgb_affect` | **NEW** | Code quality | Tiny |
| 14 | **8.** Consistent error returns | Partial | Code quality | Medium |
| 15 | **5.** Simplify `ft_file_store` | Open | Performance | Medium |
| 16 | **9.** Separate prototypes from defines | Open | Hygiene | Small |

**3 issues fixed since last review. 5 new issues introduced. 11 issues still open.**

Start from the top — the first 6 are quick wins you can knock out in 30 minutes.

---
---

# Part 2 — Becoming a Better Developer (Updated)

> Updated based on the changes you made between reviews — what improved,
> what didn't, and what new patterns I see.

---

## Your Strengths (updated)

### 1. You structure your project well from the start

Still true. The directory layout remains clean and logical. You also moved
`loading_utils.c` into `src/utils/` and kept parsing files together in
`src/parsing/`. Good instinct.

### 2. You use a garbage collector

Still a solid engineering decision. The `gc_tmp`/`gc_global` separation
works well in practice — `ft_parsing` correctly frees `gc_tmp` at the end.

### 3. You separate parsing from execution

Still true and well maintained. The pipeline in `ft_parsing` reads like a
checklist: validate, open, store, parse textures, fill map, check map, free
temps. Clean.

### 4. You respond to feedback

**New strength.** You read the first review and acted on the hardest item
first (`t_p_structs`), added the missing player check, wrote `ft_atoi_safe`
to fix the silent atoi bug, and added duplicate-color detection. That shows
you can take criticism, prioritize, and execute. Most developers get
defensive or fix only the easy stuff. You went for the architecture change.

### 5. You write defensive error handling

You consistently check return values from allocation functions and return
meaningful error codes. The `ft_error` + `ft_exit` pattern gives you clean
error reporting. You also added `ERR_MSG_NO_PLAYER` and `ERR_MSG_SET_COLOR`
— specific messages for specific failures. That's the right direction.

---

## Your Weaknesses (updated)

### W1. You leave debug artifacts in the code

**What I observed:** `printf("INT = %d\n", *nb)` sitting in production code
(parsing_textures.c:35). This is the kind of thing that slips past in
evaluation and makes a reviewer question your attention to detail.

**The habit to build:** Before every commit, run:
```bash
grep -rn "printf" src/
```
If any hit isn't a `ft_printf` or a deliberate user-facing message, delete
it. Better: create a `DEBUG_LOG` macro that compiles to nothing in release
mode, so you never have to hunt for stray prints again.

### W2. You copy-paste first, refactor never

**Still true.** `ft_texture_dispatch` still has 4 identical wall-texture
blocks. `ft_player_set` still has 4 independent `if` blocks. You even
repeated the `if/if` pattern in the new `ft_rgb_affect`.

When you added the duplicate-color check, you copy-pasted the FLOOR block
and changed "FLOOR" to "CEILING". That's a third repetition in the same
function. The Rule of Three says: it's time to extract.

**The habit hasn't changed — push yourself on this one.** Every time you
paste code, set a timer: you have 10 minutes to extract it into a helper.

### W3. You don't test your edge cases mentally

**Partially improved.** You *did* think about "what if there's no player?"
and "what if a color is set twice?" — both good catches.

But you *didn't* think about:
- "What if `ft_atoi_safe` is called without pre-zeroing `*nb`?"
- "What if the file has textures but no map?"
- "What if `ft_strncmp("FLOOR", "F", 1)` matches?" (still unfixed from v1)

You're getting better at catching *missing checks* but still missing
*function contract* edge cases. The new question to ask after writing any
function:

> "If someone calls this function without reading my implementation,
> what wrong assumptions might they make?"

### W4. You leave TODOs that never get done

**New weakness.** The `// TODO : A REFACTORISER POUR LA NORME` comment on
`ft_rgb` has been there since the first review. You knew it was a problem,
you wrote yourself a note, and you moved on to other things.

TODOs are broken promises. They feel productive when you write them — "at
least I acknowledged it" — but they accumulate into a backlog that never
gets addressed.

**The habit to build:** If a TODO takes less than 30 minutes, do it now.
If it takes longer, create a real task (write it down somewhere you'll see
it daily) and set a deadline. If you can't commit to a deadline, delete the
TODO — it's just noise.

### W5. Comment-code mismatches erode trust

**New weakness.** The comment block above `ft_player_set` says
"FT_FORMAT_CHECK - VALIDATES THAT THE FILE EXTENSION IS .CUB" — that's a
completely different function. A reader who trusts the comment will
misunderstand the code. A reader who notices the mismatch will stop
trusting *all* your comments.

**The habit to build:** When you move or rename a function, update its
comments in the same edit. Not later, not in a separate commit — right then.
If a comment doesn't match its code, delete the comment. Wrong documentation
is worse than no documentation.

---

## Habits of Developers Who Write Beautiful Code (updated)

### H1. Read code more than you write it

Still the best advice. Find 42 students who scored well on cub3d and read
their texture parsing. See how they solved the same problems you're facing.

### H2. Make the compiler your ally

Still important. One addition: consider adding `-fsanitize=address` to your
debug builds. It catches memory errors that `-Wall -Wextra` can't see.

### H3. Name things so a stranger can read your code

Improvement: your new error messages (`ERR_MSG_NO_PLAYER`, `ERR_MSG_SET_COLOR`)
are specific and helpful. That's good naming applied to errors.

Still room to improve: `arr`, `ret`, `nb`, `split` in `ft_rgb` could all
be more descriptive: `tokens`, `err`, `value`, `rgb_parts`.

### H4. Write small functions with one job

Your `ft_atoi_safe` is a good example — it does one thing (safe string-to-int
conversion) and does it well. Apply the same discipline to `ft_rgb` and
`ft_texture_dispatch`, which are still doing too many things.

### H5. Delete code you don't need

You deleted `vectors.c` and `t_p_structs` — good. Now delete
`ft_textures_detect` (redundant), the debug printf, the stale TODO, and
the misplaced comment. Keep the momentum going.

### H6. Commit often, with intention

Your recent commits show progress: they describe what changed. Next step:
commit in English consistently, and make each commit one logical change.
"Remove debug printf from rgb_convert" is a better commit than bundling it
with 5 other fixes.

### H7. Finish what you start before starting something new

**New habit.** You fixed `t_p_structs` (big win) and added `ft_atoi_safe`
(good), but left `ft_rgb` untouched despite writing yourself a TODO to fix
it. You added the player-count check but left the `ft_strncmp` n=1 bug
from the same review unfixed.

The pattern: you fix the interesting/hard problems and skip the tedious ones.
But the tedious ones are where bugs hide. Force yourself to finish all fixes
in a category before moving to the next. "All parsing bugs fixed" is more
valuable than "the hardest parsing bug fixed plus three new features."

### H8. Review your own diffs before committing

**New habit.** The debug printf, the misplaced comment, and the uninitialized
`*nb` would all be caught by spending 2 minutes reading your own `git diff`
before committing. Make it a ritual:

```bash
git diff --staged
```

Read every line. Ask: "Would I be embarrassed if a reviewer saw this?"
If yes, fix it before committing.

---

## Summary: Your Growth Roadmap (updated)

```
WHERE YOU WERE (first review)
  |
  |  Circular struct dependencies, silent bugs,
  |  no edge case handling
  |
  v
WHERE YOU ARE NOW
  |
  |  Architecture cleaned up (t_p_structs gone)
  |  Better error handling (atoi_safe, player check, color dupes)
  |  But: debug artifacts, stale TODOs, copy-paste patterns remain
  |
  v
NEXT LEVEL: Clean discipline
  |
  |  - Zero debug prints in committed code
  |  - Every TODO resolved or deleted within 48 hours
  |  - Review your own diff before every commit
  |  - Fix ALL items in a review, not just the interesting ones
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
  |  - What happens if my function is called wrong?
  |  - What inputs didn't I consider?
  |  - Does every function own its own state?
  |
  v
THE GOAL: Code that doesn't need comments because it reads like prose.
```

You've moved up one level since the first review. That's real progress.
The next level is about *discipline* more than *skill* — and discipline
is just repeated choices. You already have the skill. Now choose to apply
it consistently.
