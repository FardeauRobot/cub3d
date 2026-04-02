/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salman <salman@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 21:54:39 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/30 09:38:33 by salman           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include "defines.h"
#include "mlx.h"
#include <stdio.h>

void	ft_move_update(void *cub)
{
	t_cub	*data;

	data = (t_cub *)cub;
	if (data->player.rotating != NONE)
		ft_rotate(&data->player);
	if (data->player.moving != NONE)
		ft_move(&data->player, data->map.map);
}

void	ft_camera_update(void *cub)
{
	t_cub	*data;
	t_player *player;

	data = (t_cub *)cub;
	player = &data->player;
	player->camera_plane_x = -player->dir_y * 0.66;
	player->camera_plane_y = player->dir_x * 0.66;
}

int	ft_game_loop(void *cub)
{
	ft_move_update(cub);
	ft_camera_update(cub);
	ft_map_render(cub);
	return (SUCCESS);
}

int	ft_game_end(void *data)
{
	t_cub	*cub;

	cub = (t_cub *)data;
	if (cub->mlx)
		mlx_loop_end(cub->mlx);
	return (SUCCESS);
}

int	ft_mouse( int x, int y, void *cub)
{
	t_cub *data;
	int mid;
	(void)y;

	data = (t_cub *)cub;
	mid = data->screen_width / 2;
	if (x < mid - 150)
		data->player.rotating = LEFT;
	else if (x > mid + 150)
		data->player.rotating = RIGHT;
	else
		data->player.rotating = NONE;
	return (SUCCESS);
}

// FUNCTION USED TO START THE GAME LOOP AFTER PARSING IS DONE
void	ft_game(t_cub *data)
{
	ft_game_init(data);
	mlx_loop_hook(data->mlx, ft_game_loop, data);
	mlx_hook(data->win,6, 1L << 6, ft_mouse, data);
	mlx_hook(data->win, 2, 1L << 0, ft_press_keys, data);
	mlx_hook(data->win, 3, 1L << 1, ft_release_keys, data);
	mlx_hook(data->win, 17, 1L << 17, ft_game_end, data);
	mlx_loop(data->mlx);
	ft_data_clean(data);
}
