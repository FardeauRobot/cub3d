/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 22:44:44 by fardeau           #+#    #+#             */
/*   Updated: 2026/04/02 19:11:41 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FUNCTION USED TO PRE-RENDER THE WHOLE MAP INTO THE MINIMAP CACHE IMAGE
void	ft_minimap_cache_render(t_minimap *minimap, t_map *map)
{
	int	x;
	int	y;

	y = -1;
	while (++y < map->height)
	{
		x = -1;
		while (++x < map->width)
		{
			if (map->map[y][x] == '1')
				ft_img_to_img(&minimap->cache, &minimap->tiles[WALL].tile_img, x
					* TILE_SIZE, y * TILE_SIZE);
			else if (map->map[y][x] == '0' || map->map[y][x] == 'N'
				|| map->map[y][x] == 'S' || map->map[y][x] == 'E'
				|| map->map[y][x] == 'W')
				ft_img_to_img(&minimap->cache, &minimap->tiles[EMPTY].tile_img,
					x * TILE_SIZE, y * TILE_SIZE);
		}
	}
}

// FUNCTION USED TO STAMP THE PRE-RENDERED MINIMAP CACHE INTO THE DISPLAY
void	ft_minimap_draw(t_cub *data)
{
	ft_img_to_img(&data->display, &data->map.minimap.cache,
		data->map.minimap.offset_x, data->map.minimap.offset_y);
}

// FUNCTION USED TO DRAW THE PLAYER MARKER INTO THE DISPLAY IMAGE
void	ft_char_draw(t_cub *data)
{
	int	screen_x;
	int	screen_y;

	screen_x = data->map.minimap.offset_x + (int)(data->player.pos_x
			* TILE_SIZE) - (data->player.char_img.width / 2);
	screen_y = data->map.minimap.offset_y + (int)(data->player.pos_y
			* TILE_SIZE) - (data->player.char_img.width / 2);
	ft_img_to_img(&data->display, &data->player.char_img, screen_x, screen_y);
}

void	ft_orient_draw(t_cub *data)
{
	int	screen_x;
	int	screen_y;

	screen_x = data->map.minimap.offset_x + (int)((data->player.pos_x
				+ data->player.dir_x) * TILE_SIZE)
		- (data->player.test_view.width / 2);
	screen_y = data->map.minimap.offset_y + (int)((data->player.pos_y
				+ data->player.dir_y) * TILE_SIZE)
		- (data->player.test_view.height / 2);
	ft_img_to_img(&data->display, &data->player.test_view, screen_x, screen_y);
}

// FUNCTION USED TO RENDER EVERYTHING INTO DISPLAY AND PUSH TO WINDOW ONCE
int	ft_map_render(void *cub)
{
	t_cub	*data;

	data = (t_cub *)cub;
	ft_map_raycasting(data);
	if (data->map.minimap.display_map == ON)
	{
		ft_minimap_draw(data);
		ft_char_draw(data);
		ft_orient_draw(data);
	}
	mlx_put_image_to_window(data->mlx, data->win, data->display.img, 0, 0);
	return (SUCCESS);
}
