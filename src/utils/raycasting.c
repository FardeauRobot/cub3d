/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 17:47:49 by alamjada          #+#    #+#             */
/*   Updated: 2026/04/03 19:09:59 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FT_LIMIT_DRAW - CLAMPS THE DRAW START AND END TO SCREEN BOUNDARIES
static void	ft_limit_draw(t_cub *data, t_ray *ray, t_draw *dw)
{
	ray->draw_start = -ray->line_height / 2 + data->screen_height / 2
		+ dw->pitch;
	if (ray->draw_start < 0)
		ray->draw_start = 0;
	ray->draw_end = ray->line_height / 2 + data->screen_height / 2 + dw->pitch;
	if (ray->draw_end >= data->screen_height)
		ray->draw_end = data->screen_height - 1;
}

// FT_COMPUTE_COLLISION - COMPUTES THE EXACT WALL HIT POSITION FOR TEXTURING
static void	ft_compute_collision(t_cub *data, t_ray *ray)
{
	if (ray->side == 0)
		ray->wall_x = data->player.pos_y + ray->perp_wall_dist * ray->ray_dir_y;
	else
		ray->wall_x = data->player.pos_x + ray->perp_wall_dist * ray->ray_dir_x;
	ray->wall_x -= floor(ray->wall_x);
}

// FT_COMPUTE_DIST - COMPUTES PERPENDICULAR WALL DISTANCE AND LINE HEIGHT
static void	ft_compute_dist(t_cub *data, t_ray *ray, t_draw *dw)
{
	if (ray->side == 0)
		ray->perp_wall_dist = ray->side_dist_x - ray->delta_dist_x;
	else
		ray->perp_wall_dist = ray->side_dist_y - ray->delta_dist_y;
	ray->line_height = (int)data->screen_height / ray->perp_wall_dist;
	dw->pitch = 100;
}

// FT_DEFINE_TEX - SELECTS THE WALL TEXTURE BASED ON RAY SIDE AND DIRECTION
static void	ft_define_tex(t_cub *data, t_ray *ray, t_draw *dw)
{
	if (ray->side == 0)
	{
		if (ray->ray_dir_x > 0)
			dw->tex = &data->textures.wall_w;
		else
			dw->tex = &data->textures.wall_e;
	}
	else
	{
		if (ray->ray_dir_y > 0)
			dw->tex = &data->textures.wall_n;
		else
			dw->tex = &data->textures.wall_s;
	}
}

// FT_DRAW_RAY - DRAWS A FULL VERTICAL STRIPE FOR ONE RAY COLUMN
void	ft_draw_ray(t_cub *data, t_ray *ray, int x)
{
	t_draw	dw;

	ft_compute_dist(data, ray, &dw);
	ft_limit_draw(data, ray, &dw);
	ft_compute_collision(data, ray);
	ft_define_tex(data, ray, &dw);
	ft_get_x_tex(ray, &dw);
	ft_get_y_tex(data, ray, &dw);
	ft_draw_ceiling(data, ray, x);
	ft_draw_wall(data, ray, &dw, x);
	ft_draw_floor(data, ray, x);
}
