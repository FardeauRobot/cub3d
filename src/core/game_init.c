/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 11:55:56 by tibras            #+#    #+#             */
/*   Updated: 2026/04/02 19:14:36 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// FILE IN CHARGE OF THE INITIALIZATION OF THE GAME
#include "cub3d.h"

// INITIALIZES THE WALL SPRITES
void	ft_sprites_init(t_cub *data)
{
	if (ft_xpm_img(data, &data->textures.wall_n, data->textures.north) != 0)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, data->textures.north);
	if (ft_xpm_img(data, &data->textures.wall_s, data->textures.south) != 0)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, data->textures.south);
	if (ft_xpm_img(data, &data->textures.wall_e, data->textures.east) != 0)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, data->textures.east);
	if (ft_xpm_img(data, &data->textures.wall_w, data->textures.west) != 0)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, data->textures.west);
}

// FUNCTION USED TO RETURN THE DISPLAY COLOR ASSOCIATED WITH A TILE TYPE
int	ft_tile_color_get(t_etile content)
{
	if (content == WALL)
		return (WALL_TILE_COL);
	return (EMPTY_TILE_COL);
}

// FUNCTION USED TO INITIALIZE ONE TILE IMAGE WITH THE RIGHT COLOR
void	ft_tiles_init(t_cub *data, t_tile *tile, t_etile content)
{
	ft_img_init(data, &tile->tile_img, TILE_SIZE, TILE_SIZE);
	tile->tile_img.color = ft_tile_color_get(content);
	ft_img_fill(&tile->tile_img, tile->tile_img.height, tile->tile_img.width,
		tile->tile_img.color);
}

// FUNCTION USED TO INITIALIZE AN MLX IMAGE AND ITS RAW PIXEL BUFFER
void	ft_img_init(t_cub *data, t_img *img, int width, int height)
{
	img->height = height;
	img->width = width;
	img->img = mlx_new_image(data->mlx, img->width, img->height);
	if (!img->img)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_FAIL_MLX);
	img->addr = mlx_get_data_addr(img->img, &img->bpp, &img->line_len,
			&img->endian);
	if (!img->addr)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_FAIL_MLX);
}

// INITALIZES ALL THE NECESSARY TOOLS FOR MLX TO WORK PROPERLY
void	ft_mlx_init(t_cub *data)
{
	data->mlx = mlx_init();
	if (!data->mlx)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_MSG_MLX);
	if (mlx_get_screen_size(data->mlx, &data->screen_width,
			&data->screen_height))
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_MSG_MLX);
	data->screen_height *= DISP_SCALE;
	data->screen_width *= DISP_SCALE;
	data->win = mlx_new_window(data->mlx, data->screen_width,
			data->screen_height, WIN_NAME);
	if (!data->win)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_MSG_MLX);
	data->display.width = data->screen_width;
	data->display.height = data->screen_height;
	data->display.img = mlx_new_image(data->mlx, data->display.width,
			data->display.height);
	if (!data->display.img)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_FAIL_MLX);
	data->display.addr = mlx_get_data_addr(data->display.img,
			&data->display.bpp, &data->display.line_len, &data->display.endian);
	if (!data->display.addr)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_FAIL_MLX);
}

void	ft_game_init(t_cub *data)
{
	ft_mlx_init(data);
	ft_minimap_init(data);
	ft_char_init(data);
	ft_sprites_init(data);
}
