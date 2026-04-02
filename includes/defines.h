/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/04/02 12:48:40 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINES_H
# define DEFINES_H

# include "structures.h"

/* ============== RETURN CODES ============================= */
# define SUCCESS 0
# define FAILURE 1

/* ============== GAME MACROS ============================= */

# define DISP_SCALE 0.75 
# define WIN_NAME "cub3d"

# define TILE_SIZE 16
# define CHAR_SIZE 4

# define	CHAR_SPEED 0.05
# define	ROT_SPEED 0.05

// BITFLAGS 
# define UP 0b0001
# define DOWN 0b0010
# define LEFT 0b0100
# define RIGHT 0b1000

/* ============== COLORS ============================= */
# define EMPTY_TILE_COL 0xFFFFFF
# define WALL_TILE_COL 0xD8A9DF
# define CHAR_COL 0xFF0000

/* ============== CORE ============================= */
// GAME_INIT.C
void	ft_tiles_init(t_cub *data, t_tile *tile, t_etile content);
void	ft_game_init(t_cub *data);

// GAME.C
void	ft_game(t_cub *data);

/* ============== PARSING ============================= */
// PARSING.C
int		ft_parsing(t_cub *data, char **argv, int argc);

// PARSING_TESTURES.C
int		ft_file_store(t_cub *data);
int		ft_textures_parsing(t_cub *data);

/* ============== RENDER ============================= */
// RENDER_MAP.C
void	ft_minimap_cache_render(t_minimap *minimap, t_map *map);
void	ft_minimap_draw(t_cub *data);
void	ft_char_draw(t_cub *data);
int		ft_map_render(void *cub);

// RENDER_UTILS.C
void	ft_img_pixel_put(t_img *img, int x, int y, int color);
int		ft_img_pixel_get(t_img *img, int x, int y);
void	ft_img_to_img(t_img *dst, t_img *src, int dst_x, int dst_y);
void	ft_img_fill(t_img *img, int height, int width, int color);

// IMGS_INIT.C
void	ft_img_init(t_cub *data, t_img *img, int width, int height);
void	ft_minimap_init(t_cub *data);
void	ft_char_init(t_cub *data);

/* ============== UTILS ============================= */
// OUTPUT.C
void	ft_matrix_print(char **matrix);
void	ft_cub_print(t_cub *data);

// PARSING_UTILS.C
void	ft_player_set(t_player *player, int x, int y, char orient);
int		ft_format_check(char *filepath);

// ERROR.C
void	ft_data_clean(t_cub *data);

// LOADING_UTILS.C
int		ft_xpm_img(t_cub *game, t_img *img, char *path);
void	ft_mlx_init(t_cub *data);

// KEYS.C
int		ft_move(t_player *player, char **map);
void	ft_rotate(t_player *player);
int		ft_release_keys(int keycode, void *cub);
int		ft_press_keys(int keycode, void *cub);

#endif
