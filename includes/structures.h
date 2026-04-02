/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structures.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/04/02 19:08:29 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTURES_H
# define STRUCTURES_H

# include "defines.h"
# include "libft.h"

/* ============== ENUM =============================== */

typedef enum e_background
{
	FLOOR,
	CEILING
}	t_background;

typedef enum e_etile
{
	EMPTY,
	WALL
}	t_etile;

typedef enum e_switch
{
	ON,
	OFF
}	t_switch;

/* ============== STRUCTURES =============================== */

/*
** t_data - main project context
** Centralises all runtime data so every function can access it.
*/

/* RECALL FOR STRUCTURES USED BELOW THEIR CHILDREN */
typedef struct s_map		t_map;
typedef struct s_cub		t_cub;
typedef struct s_player		t_player;
typedef struct s_textures	t_textures;
typedef struct s_minimap	t_minimap;
typedef struct s_img t_img;
typedef struct s_ray t_ray;
typedef struct s_draw t_draw;

typedef struct s_draw
{
	int				pitch;
	t_img			*tex;
	int				tex_width;
	int				tex_x;
	int				tex_height;
	double			step;
	double			tex_pos;
	int				y;
	int				tex_y;
	char			*pixel;
	unsigned int	color;
} t_draw;

typedef struct s_ray
{
	double	camera_x;
	double	ray_dir_x;
	double	ray_dir_y;
	int		map_x;
	int		map_y;
	double	side_dist_x;
	double	side_dist_y;
	double	delta_dist_x;
	double	delta_dist_y;
	double	perp_wall_dist;
	double	wall_x;
	int		step_x;
	int		step_y;
	int		hit;
	int		side;
	int		line_height;
	int		draw_start;
	int		draw_end;
}			t_ray;

/* STRUCTURE USED TO STORE THE DIFFERENT INFOS ABOUT AN IMG */
typedef struct s_img
{
	void	*img;
	char	*addr;
	int		bpp;
	int		line_len;
	int		endian;
	int		width;
	int		height;
	int		color;
}	t_img;

/* STRUCTURE USED TO STORE ALL THE DIFFERENT TEXTURES INFO */
typedef struct s_textures
{
	t_img	empty_tile_map;
	t_img	wall_tile_map;
	char	*north;
	t_img	wall_n;
	char	*south;
	t_img	wall_s;
	char	*east;
	t_img	wall_e;
	char	*west;
	t_img	wall_w;
	int		floor_rgb;
	int		ceiling_rgb;
}	t_textures;

/* STRUCTURE USED TO PRINT TILES OF THE MINIMAP */
typedef struct s_tile
{
	t_img	tile_img;
	int		y;
	int		x;
}	t_tile;

/* STRUCTURE USED TO STORE ALL THE MINIMAP INFOS -> NESTED IN MAP */
typedef struct s_minimap
{
	t_img	cache;
	int		offset_x;
	int		offset_y;
	int		display_map;
	t_tile	tiles[2];
}	t_minimap;

/* STRUCTURE USED TO STORE ALL THE MAP INFOS */
typedef struct s_map
{
	int			index_map_start;
	char		**map;
	int			height;
	int			width;
	t_minimap	minimap;
}	t_map;

/* STRUCTURE USED TO STORE ALL THE PLAYER INFOS */
typedef struct s_player
{
	t_img			char_img;
	t_img			test_view;
	double			pos_x;
	double			pos_y;
	double			dir_x;
	double			dir_y;
	double			camera_plane_x;
	double			camera_plane_y;
	int				rotating;
	int				moving;
}	t_player;

/* STRUCTURE USED AS THE MAIN ACCESS POINT */
typedef struct s_cub
{
	void			*mlx;
	void			*win;
	t_img			display;
	int				screen_width;
	int				screen_height;
	char			**file;
	int				fd_map;
	t_map			map;
	t_player		player;
	t_textures		textures;
	t_list			*gc_global;
	t_list			*gc_tmp;
}	t_cub;

#endif
