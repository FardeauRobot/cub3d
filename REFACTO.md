# REFACTO - cub3D

---

## 1. ft_rgb (parsing_textures.c) - HIGH PRIORITY

Le TODO le dit deja : la chaine de `if/else if` sur `rgb[0]`, `rgb[1]`, `rgb[2]`
est repetitive. Un simple index qui s'incremente resout tout.

**Avant :**
```c
if (split[j][0] && rgb[0] == -1)
{
    if (ft_rgb_convert(split[j], &rgb[0]) != SUCCESS)
        return (FAILURE);
}
else if (split[j][0] && rgb[1] == -1)
{
    if (ft_rgb_convert(split[j], &rgb[1]) != SUCCESS)
        return (FAILURE);
}
else if (split[j][0] && rgb[2] == -1)
    if (ft_rgb_convert(split[j], &rgb[2]) != SUCCESS)
        return (FAILURE);
```

**Apres :**
```c
int	ft_rgb(t_cub *data, char **arr, t_background part)
{
    int		rgb[3];
    int		idx;
    int		i;
    int		j;
    char	**split;

    idx = 0;
    i = -1;
    while (arr[++i])
    {
        j = -1;
        split = ft_split_charset_gc(arr[i], ", \t", &data->gc_tmp);
        if (!split)
            return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
        while (split[++j])
        {
            if (i == 0 && j == 0)
                split[j] = ft_strtrim_gc(split[j], "FC", &data->gc_tmp);
            if (!split[j][0])
                continue ;
            if (idx >= 3)
                return (ft_error(ERR_MSG_INVALID_RGB, "too many values", ERRN_PARSING));
            if (ft_rgb_convert(split[j], &rgb[idx++]) != SUCCESS)
                return (FAILURE);
        }
    }
    if (idx != 3)
        return (ft_error(ERR_MSG_INVALID_RGB, "not enough values", ERRN_PARSING));
    ft_rgb_affect(data, rgb, part);
    return (SUCCESS);
}
```

- Plus de `-1` sentinelle dans `rgb[]`, juste un `idx` qui avance.
- On valide aussi qu'il y a exactement 3 composantes (l'original ne le fait pas).
- Respecte la norme 42 (~25 lignes).

---

## 2. ft_texture_dispatch (parsing_textures.c) - MEDIUM

Trop de `if` en serie, chacun avec le meme pattern strdup + check NULL.
On peut factoriser l'affectation des murs dans un helper, et utiliser `else if`
pour eviter les comparaisons inutiles.

**Helper possible :**
```c
static int	ft_assign_wall(t_cub *data, char **dst, char *path)
{
    *dst = ft_strdup_gc(path, &data->gc_global);
    if (!*dst)
        return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
    return (SUCCESS);
}
```

**Utilisation :**
```c
static int	ft_texture_dispatch(t_cub *data, char **arr, char *id, char *path)
{
    if (ft_strncmp(id, "NO", 2) == 0)
        return (ft_assign_wall(data, &data->textures.north, path));
    else if (ft_strncmp(id, "SO", 2) == 0)
        return (ft_assign_wall(data, &data->textures.south, path));
    else if (ft_strncmp(id, "WE", 2) == 0)
        return (ft_assign_wall(data, &data->textures.west, path));
    else if (ft_strncmp(id, "EA", 2) == 0)
        return (ft_assign_wall(data, &data->textures.east, path));
    else if (id[0] == 'F')
        // ... gestion floor
    else if (id[0] == 'C')
        // ... gestion ceiling
    return (SUCCESS);
}
```

Passe de ~45 lignes a ~25.

---

## 3. ft_destroy_imgs (error.c) - LOW EFFORT

10 blocs identiques `if (x.img) mlx_destroy_image(...)`. Un petit helper :

```c
static void	ft_safe_destroy(void *mlx, t_img *img)
{
    if (img->img)
        mlx_destroy_image(mlx, img->img);
}
```

Puis :
```c
void	ft_destroy_imgs(t_cub *data)
{
    if (!data->mlx)
        return ;
    ft_safe_destroy(data->mlx, &data->display);
    ft_safe_destroy(data->mlx, &data->map.minimap.cache);
    ft_safe_destroy(data->mlx, &data->map.minimap.tiles[0].tile_img);
    ft_safe_destroy(data->mlx, &data->map.minimap.tiles[1].tile_img);
    ft_safe_destroy(data->mlx, &data->player.char_img);
    ft_safe_destroy(data->mlx, &data->player.test_view);
    ft_safe_destroy(data->mlx, &data->textures.wall_n);
    ft_safe_destroy(data->mlx, &data->textures.wall_s);
    ft_safe_destroy(data->mlx, &data->textures.wall_e);
    ft_safe_destroy(data->mlx, &data->textures.wall_w);
}
```

Le `if (!data->mlx)` en haut supprime le check a chaque ligne.

---

## 4. ft_img_fill (render_utils.c) - LOW EFFORT / HIGH IMPACT

`ft_img_fill` appelle `ft_img_pixel_put` pixel par pixel, qui fait 5 checks
de bounds **a chaque pixel**. Pour un remplissage complet on peut directement
ecrire dans le buffer avec `ft_memset` ou une boucle directe :

```c
void	ft_img_fill(t_img *img, int height, int width, int color)
{
    int				x;
    int				y;
    unsigned int	*line;

    if (!img || !img->addr)
        return ;
    y = -1;
    while (++y < height)
    {
        line = (unsigned int *)(img->addr + y * img->line_len);
        x = -1;
        while (++x < width)
            line[x] = color;
    }
}
```

Ca supprime les bounds-checks redondants et le calcul d'offset repete.
Meme logique applicable a `ft_draw_ceiling` et `ft_draw_floor`
dans raycasting_utils.c.

---

## 5. ft_player_set (parsing_utils.c) - COSMETIC

4 blocs `if` pour N/S/E/W. Pas grave mais `else if` evite des comparaisons
inutiles et rend le flux plus clair.

---

## Quick tips

- **`else if` partout** : `ft_texture_dispatch`, `ft_textures_detect`,
  `ft_player_set`, `ft_rotate` - a chaque fois qu'un seul `if` peut matcher,
  utiliser `else if` pour eviter de tester les cas restants.

- **`NONE` pour rotating** : `NONE` n'est pas defini dans vos enums/defines.
  Ca compile parce que `NONE` vaut `0` via un define implicite ou une enum
  quelque part dans votre libft/mlx. Definissez-le explicitement pour la clarte
  (par exemple `# define NONE 0` dans defines.h).

- **`dw->pitch = 100` hardcode** dans `ft_compute_dist` : un `# define PITCH 100`
  dans defines.h serait plus propre et facile a tweaker.

- **Le magic number `8355711`** dans `ft_draw_wall` c'est `0x7F7F7F`.
  Un `# define SHADOW_MASK 0x7F7F7F` rendrait le code plus lisible.

- **`0.66` dans `ft_camera_update`** : c'est le FOV factor. Un define
  `FOV_FACTOR` rend l'intention explicite.

- **`ft_img_to_img`** est lent pixel-par-pixel avec double bounds-check
  (get + put). Pour la minimap qui se re-blitte chaque frame, un `ft_memcpy`
  ligne par ligne serait beaucoup plus rapide.

---

## Erreurs / Bugs detectes

### 1. ft_rgb_convert ne valide pas la range 0-255
`ft_rgb_convert` verifie `strlen <= 3` et que c'est des digits, mais `"999"` ou
`"256"` passent sans probleme. Il faut ajouter un check :
```c
if (*nb < 0 || *nb > 255)
{
    *nb = -1;
    return (ft_error(ERR_MSG_INVALID_RGB, str, ERRN_PARSING));
}
```

### 2. ft_perf_dda - off-by-one dans le bounds check
```c
if ((ray->map_y < 0 || ray->map_y > data->map.height) || ...)
```
Devrait etre `>=` et non `>`. `map.height` est le nombre de lignes,
donc `map[map.height]` est le NULL terminator. Access out-of-bounds.
```c
if (ray->map_y < 0 || ray->map_y >= data->map.height
    || ray->map_x < 0 || ray->map_x >= data->map.width)
```
De plus, il faut aussi verifier que `ray->map_x < (int)ft_strlen(map[ray->map_y])`
car les lignes de la map ne sont pas toutes de la meme longueur.

### 3. ft_data_clean ferme stdin si fd_map == 0
`data` est initialise avec `ft_bzero`, donc `fd_map == 0`. Si `ft_exit` est
appele avant l'ouverture du fichier, `close(0)` ferme stdin.
**Fix :** initialiser `data->fd_map = -1` dans main, ou checker avant :
```c
if (data->fd_map > 0)
    close(data->fd_map);
```

### 4. ft_char_draw utilise width au lieu de height pour Y
```c
screen_y = ... - (data->player.char_img.width / 2);  // <- devrait etre .height
```
Ca marche parce que char_img est carre (CHAR_SIZE x CHAR_SIZE), mais c'est
techniquement faux.

### 5. ft_rotate - `rot` potentiellement non initialise
Si `rotating` n'est ni `NONE`, ni `LEFT`, ni `RIGHT`, la variable `rot` est
utilisee sans etre initialisee. Peu probable mais un `else` suffit :
```c
if (player->rotating == LEFT)
    rot = -ROT_SPEED;
else
    rot = ROT_SPEED;
```

### 6. ft_move - collision check ne permet pas le wall sliding
La collision est verifiee a `(pos_x + move_x, pos_y + move_y)` en un seul test.
Si le joueur se deplace en diagonale contre un mur, il est bloque des deux axes.
Verifier X et Y separement permet de "glisser" le long des murs :
```c
if (ft_ischarset(map[(int)player->pos_y]
    [(int)(player->pos_x + move_x * CHAR_SPEED)], "0NSEW"))
    player->pos_x += move_x * CHAR_SPEED;
if (ft_ischarset(map[(int)(player->pos_y + move_y * CHAR_SPEED)]
    [(int)player->pos_x], "0NSEW"))
    player->pos_y += move_y * CHAR_SPEED;
```

### 7. ft_textures_detect - prefix match trop permissif
`ft_strncmp(id, "F", 1)` matche n'importe quelle string qui commence par 'F'.
En pratique ca fonctionne parce que le split isole l'identifiant, mais
`ft_strcmp` ou comparer aussi la longueur serait plus robuste.

### 8. Duplicate textures non detectees
`ft_texture_dispatch` ecrase silencieusement un path deja assigne si le meme
identifiant (NO, SO, etc.) apparait deux fois dans le fichier .cub. Le subject
considere ca comme une erreur. Ajouter un check comme pour F/C :
```c
if (data->textures.north != NULL)
    return (ft_error(ERR_MSG_SET_COLOR, "NO", ERRN_TEXTURES));
```
