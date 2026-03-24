/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 22:44:44 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/24 15:46:15 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <sys/time.h>

double ft_get_time()
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec + tv.tv_usec / 1000000.0);
}

void ft_map_raycasting(t_cub *data)
{
	double pos_x = data->player.pos_x;
	double pos_y = data->player.pos_y;
	double dir_x = data->player.dir_x;
	double dir_y = data->player.dir_y;
	double plane_x = data->player.camera_plane_x;
	double plane_y = data->player.camera_plane_y;
	double time = 0;
	double old_time = 0;

	int x = 0;
	while (x < data->screen_width)
	{
		double camera_x = 2 * x / (double)data->screen_width - 1;
		double ray_dir_x = dir_x + plane_x * camera_x;
		double ray_dir_y = dir_y + plane_y * camera_x;
		int map_x = (int)pos_x;
		int map_y = (int)pos_y;
		double side_dist_x;
		double side_dist_y;
		double delta_dist_x = fabs(1 / ray_dir_x);
		double delta_dist_y = fabs(1 / ray_dir_y);
		double perp_wall_dist;
		int step_x;
		int step_y;
		int hit = 0;
		int side;
		if (ray_dir_x < 0)
		{
			step_x = -1;
			side_dist_x = (pos_x - map_x) * delta_dist_x;
		}
		else
		{
			step_x = 1;
			side_dist_x = (map_x + 1.0 - pos_x) * delta_dist_x;
		}
		if (ray_dir_y < 0)
		{
			step_y = -1;
			side_dist_y = (pos_y - map_y) * delta_dist_y;
		}
		else
		{
			step_y = 1;
			side_dist_y = (map_y + 1.0 - pos_y) * delta_dist_y;
		}
		while (hit == 0)
		{
			if (side_dist_x < side_dist_y)
			{
				side_dist_x += delta_dist_x;
				map_x += step_x;
				side = 0;
			}
			else
			{
				side_dist_y += delta_dist_y;
				map_y += step_y;
				side = 1;
			}
			if ((map_y < 0 || map_y > data->map.height) || (map_x < 0 || map_x > data->map.width) )
			{
				hit = 1;
				break;
			}
			if (data->map.map[map_y][map_x] == '1')
				hit = 1;
		}
		if (side == 0)				perp_wall_dist = (map_x - pos_x + (1 - step_x) / 2) / ray_dir_x;
		else						perp_wall_dist = (map_y - pos_y + (1 - step_y) / 2) / ray_dir_y;
		int line_height = (int)(data->screen_height / perp_wall_dist);
		int draw_start = -line_height / 2 + data->screen_height / 2;
		if (draw_start < 0)
			draw_start = 0;
		int draw_end = line_height / 2 + data->screen_height / 2;
		if (draw_end >= data->screen_height)
			draw_end = data->screen_height - 1;
		int color;
		if (side == 0)
			color = (ray_dir_x > 0) ? 0xFF0000 : 0x800000;
		else
			color = (ray_dir_y > 0) ? 0x00FF00 : 0x008000;
		for (int y = draw_start; y < draw_end; y++)
			ft_img_pixel_put(&data->display, x, y, color);
		old_time = time;
		time = ft_get_time();
		// double frame_time = time - old_time;
		// printf("FPS: %f\n", 1.0 / frame_time);
		// printf("Player Position: (%f, %f)\n", pos_x, pos_y);
			x++;
}
}

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
	ft_img_fill(&data->display,data->display.height, data->display.width, 0xFFB700);
	ft_img_fill(&data->display, data->display.height / 2, data->display.width, 0x57C4E5);
	ft_map_raycasting(data);
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