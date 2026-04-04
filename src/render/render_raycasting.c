/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_raycasting.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 19:10:43 by alamjada          #+#    #+#             */
/*   Updated: 2026/04/03 19:07:48 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FT_INIT_RAY - INITIALIZES RAY DIRECTION AND MAP POSITION FOR A COLUMN
static void	ft_init_ray(t_cub *data, t_ray *ray, int x)
{
	ray->camera_x = 2 * x / (double)data->screen_width - 1;
	ray->ray_dir_x = data->player.dir_x + data->player.camera_plane_x
		* ray->camera_x;
	ray->ray_dir_y = data->player.dir_y + data->player.camera_plane_y
		* ray->camera_x;
	ray->map_x = (int)data->player.pos_x;
	ray->map_y = (int)data->player.pos_y;
	ray->delta_dist_x = fabs(1 / ray->ray_dir_x);
	ray->delta_dist_y = fabs(1 / ray->ray_dir_y);
	ray->hit = 0;
	ray->wall_x = 0;
}

// FT_COMPUTE_STEP - COMPUTES STEP DIRECTION AND INITIAL SIDE DISTANCES
static void	ft_compute_step(t_cub *data, t_ray *ray)
{
	if (ray->ray_dir_x < 0)
	{
		ray->step_x = -1;
		ray->side_dist_x = (data->player.pos_x - ray->map_x)
			* ray->delta_dist_x;
	}
	else
	{
		ray->step_x = 1;
		ray->side_dist_x = (ray->map_x + 1.0 - data->player.pos_x)
			* ray->delta_dist_x;
	}
	if (ray->ray_dir_y < 0)
	{
		ray->step_y = -1;
		ray->side_dist_y = (data->player.pos_y - ray->map_y)
			* ray->delta_dist_y;
	}
	else
	{
		ray->step_y = 1;
		ray->side_dist_y = (ray->map_y + 1.0 - data->player.pos_y)
			* ray->delta_dist_y;
	}
}

// FT_PERF_DDA - PERFORMS THE DDA ALGORITHM TO FIND THE NEXT WALL HIT
static void	ft_perf_dda(t_cub *data, t_ray *ray)
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
		if ((ray->map_y < 0 || ray->map_y > data->map.height) || (ray->map_x < 0
				|| ray->map_x > data->map.width))
		{
			ray->hit = 1;
			break ;
		}
		if (data->map.map[ray->map_y][ray->map_x] == '1')
			ray->hit = 1;
	}
}

// FT_MAP_RAYCASTING - CASTS A RAY FOR EACH SCREEN COLUMN AND DRAWS WALLS
void	ft_map_raycasting(t_cub *data)
{
	t_ray	ray;
	int		x;

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
