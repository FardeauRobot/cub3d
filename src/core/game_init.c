/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 11:55:56 by tibras            #+#    #+#             */
/*   Updated: 2026/04/03 12:21:50 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// FILE IN CHARGE OF THE INITIALIZATION OF THE GAME
#include "cub3d.h"

// FUNCTION USED TO RETURN THE DISPLAY COLOR ASSOCIATED WITH A TILE TYPE
int	ft_tile_color_get(t_etile content)
{
	if (content == WALL)
		return (WALL_TILE_COL);
	return (EMPTY_TILE_COL);
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

// FT_GAME_INIT - INITIALIZES ALL GAME COMPONENTS BEFORE THE GAME LOOP
void	ft_game_init(t_cub *data)
{
	ft_mlx_init(data);
	ft_minimap_init(data);
	ft_char_init(data);
	ft_sprites_init(data);
}
