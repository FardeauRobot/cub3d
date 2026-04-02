/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 22:44:44 by fardeau           #+#    #+#             */
/*   Updated: 2026/04/02 15:04:42 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <sys/time.h>

typedef struct s_ray
{
	double camera_x;
	double ray_dir_x;
	double ray_dir_y;
	int map_x;
	int map_y;
	double side_dist_x;
	double side_dist_y;
	double delta_dist_x;
	double delta_dist_y;
	double perp_wall_dist;
	int step_x;
	int step_y;
	int hit;
	int side;
	int line_height;
	int draw_start;
	int draw_end;
} t_ray;

double ft_get_time()
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec + tv.tv_usec / 1000000.0);
}

int ft_define_color(double ray_dir_x, double ray_dir_y, int side)
{
	int color;

	if (side == 0)
		color = (ray_dir_x > 0) ? 0xFF0000 : 0x800000;
	else
		color = (ray_dir_y > 0) ? 0x00FF00 : 0x008000;
	return color;
}

void ft_init_ray(t_cub *data, t_ray *ray, int x)
{
		ray->camera_x = 2 * x / (double)data->screen_width - 1;
		ray->ray_dir_x = data->player.dir_x + data->player.camera_plane_x * ray->camera_x;
		ray->ray_dir_y = data->player.dir_y + data->player.camera_plane_y * ray->camera_x;
		ray->map_x = (int)data->player.pos_x;
		ray->map_y = (int)data->player.pos_y;
		ray->delta_dist_x = fabs(1 / ray->ray_dir_x);
		ray->delta_dist_y = fabs(1 / ray->ray_dir_y);
		ray->hit = 0;
}

void ft_compute_step(t_cub *data, t_ray *ray)
{
		if (ray->ray_dir_x < 0)
		{
			ray->step_x = -1;
			ray->side_dist_x = (data->player.pos_x - ray->map_x) * ray->delta_dist_x;
		}
		else
		{
			ray->step_x = 1;
			ray->side_dist_x = (ray->map_x + 1.0 - data->player.pos_x) * ray->delta_dist_x;
		}
		if (ray->ray_dir_y < 0)
		{
			ray->step_y = -1;
			ray->side_dist_y = (data->player.pos_y - ray->map_y) * ray->delta_dist_y;
		}
		else
		{
			ray->step_y = 1;
			ray->side_dist_y = (ray->map_y + 1.0 - data->player.pos_y) * ray->delta_dist_y;
		}

}

void ft_perf_dda(t_cub *data, t_ray *ray)
{
		while (ray->hit == 0)
		{
			if (ray->side_dist_x < ray->side_dist_y)
			{
				ray->side_dist_x += ray->delta_dist_x;
				ray->map_x += ray->step_x;
				ray->side = 0;
			}
			else
			{
				ray->side_dist_y += ray->delta_dist_y;
				ray->map_y += ray->step_y;
				ray->side = 1;
			}
			if ((ray->map_y < 0 || ray->map_y > data->map.height) || (ray->map_x < 0 || ray->map_x > data->map.width) )
			{
				ray->hit = 1;
				break;
			}
			if (data->map.map[ray->map_y][ray->map_x] == '1')
				ray->hit = 1;
		}
}

void ft_draw_ray(t_cub *data, t_ray *ray, int x)
{
	int y;

	if (ray->side == 0)
		ray->perp_wall_dist = (ray->map_x - data->player.pos_x + (1 - ray->step_x) / 2) / ray->ray_dir_x;
	else
		ray->perp_wall_dist = (ray->map_y - data->player.pos_y + (1 - ray->step_y) / 2) / ray->ray_dir_y;
	ray->line_height = (int)(data->screen_height / ray->perp_wall_dist);
	ray->draw_start = -ray->line_height / 2 + data->screen_height / 2;
	if (ray->draw_start < 0)
		ray->draw_start = 0;
	ray->draw_end = ray->line_height / 2 + data->screen_height / 2;
	if (ray->draw_end >= data->screen_height)
		ray->draw_end = data->screen_height - 1;
	y = ray->draw_start;
	while (y < ray->draw_end)
	{
		ft_img_pixel_put(&data->display, x, y, ft_define_color(ray->ray_dir_x, ray->ray_dir_y, ray->side));
		y++;
	}
}

void ft_map_raycasting(t_cub *data)
{
	t_ray ray;
	int x;

	x = 0;
	while (x < data->screen_width)
	{
		ft_init_ray(data, &ray, x);
		ft_compute_step(data, &ray);
		ft_perf_dda(data, &ray);
		ft_draw_ray(data, &ray, x);
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
