/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 22:44:44 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/18 15:54:43 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FUNCTION USED TO STAMP THE PRE-RENDERED MINIMAP CACHE INTO THE DISPLAY
void	ft_minimap_draw(t_minimap *minimap)
{
	ft_img_to_img(&minimap->p_structs->p_cub->display, &minimap->cache,
		minimap->offset_x, minimap->offset_y);
}

// FUNCTION USED TO DRAW THE PLAYER MARKER INTO THE DISPLAY IMAGE
void	ft_char_draw(t_player *player)
{
	int	screen_x;
	int	screen_y;

	screen_x = player->p_structs->p_minimap->offset_x
		+ (int)(player->pos_x * TILE_SIZE)
		- (player->char_img.width / 2);
	screen_y = player->p_structs->p_minimap->offset_y
		+ (int)(player->pos_y * TILE_SIZE)
		- (player->char_img.width / 2);
	ft_img_to_img(&player->p_structs->p_cub->display,
		&player->char_img, screen_x, screen_y);
}

void	ft_orient_draw(t_player *player)
{
	int	screen_x;
	int	screen_y;

	screen_x = player->p_structs->p_minimap->offset_x
		+ (int)((player->pos_x + player->dir_x) * TILE_SIZE)
		- (player->test_view.width / 2);
	screen_y = player->p_structs->p_minimap->offset_y
		+ (int)((player->pos_y + player->dir_y) * TILE_SIZE)
		- (player->test_view.height / 2);
	ft_img_to_img(&player->p_structs->p_cub->display,
		&player->test_view, screen_x, screen_y);
}

// FUNCTION USED TO RENDER EVERYTHING INTO DISPLAY AND PUSH TO WINDOW ONCE
int	ft_map_render(void *cub)
{
	t_cub	*data;

	data = (t_cub *)cub;
	ft_img_fill(&data->display, 0x000000);
	if (data->map.minimap.display_map == ON)
	{
		ft_minimap_draw(&data->map.minimap);
		ft_char_draw(&data->player);
		ft_orient_draw(&data->player);
	}
	mlx_put_image_to_window(data->mlx, data->win,
		data->display.img, 0, 0);
	return (SUCCESS);
}