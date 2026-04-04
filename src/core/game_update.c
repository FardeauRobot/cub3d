/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_update.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 12:17:10 by tibras            #+#    #+#             */
/*   Updated: 2026/04/03 12:21:56 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FT_MOVE_UPDATE - UPDATES PLAYER POSITION BASED ON CURRENT MOVEMENT STATE
void	ft_move_update(void *cub)
{
	t_cub	*data;

	data = (t_cub *)cub;
	if (data->player.rotating != NONE)
		ft_rotate(&data->player);
	if (data->player.moving != NONE)
		ft_move(&data->player, data->map.map);
}

// FT_CAMERA_UPDATE - UPDATES THE CAMERA PLANE BASED ON PLAYER DIRECTION
void	ft_camera_update(void *cub)
{
	t_cub		*data;
	t_player	*player;

	data = (t_cub *)cub;
	player = &data->player;
	player->camera_plane_x = -player->dir_y * 0.66;
	player->camera_plane_y = player->dir_x * 0.66;
}
