/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   imgs_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 08:49:47 by tibras            #+#    #+#             */
/*   Updated: 2026/03/30 11:59:16 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FUNCTION USED TO INITIALIZE ALL IMAGES NEEDED BY THE MINIMAP
void	ft_minimap_init(t_cub *data)
{
	t_map	*map;

	map = &data->map;
	map->minimap.display_map = OFF;
	map->minimap.offset_x = data->screen_width
		- map->width * TILE_SIZE - 10;
	map->minimap.offset_y = 10;
	ft_tiles_init(data, &map->minimap.tiles[EMPTY], EMPTY);
	ft_tiles_init(data, &map->minimap.tiles[WALL], WALL);
	ft_img_init(data, &map->minimap.cache,
		map->width * TILE_SIZE, map->height * TILE_SIZE);
	ft_minimap_cache_render(&map->minimap, map);
}

// FUNCTION USED TO INITIALIZE THE PLAYER MARKER IMAGE FOR THE MINIMAP
void	ft_char_init(t_cub *data)
{
	t_player	*character;

	character = &data->player;
	character->moving = NONE;
	character->rotating = NONE;
	character->char_img.color = CHAR_COL;
	ft_img_init(data, &character->char_img, CHAR_SIZE, CHAR_SIZE);
	ft_img_init(data, &character->test_view, CHAR_SIZE / 2, CHAR_SIZE / 2);
	ft_img_fill(&character->char_img, character->char_img.height, character->char_img.width, character->char_img.color);
	ft_img_fill(&character->test_view, character->test_view.height, character->test_view.width, character->char_img.color);
}
