/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 22:44:44 by fardeau           #+#    #+#             */
/*   Updated: 2026/04/02 17:21:47 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <sys/time.h>

typedef struct s_ray
{
	double	camera_x;
	double	ray_dir_x;
	double	ray_dir_y;
	int		map_x;
	int		map_y;
	double	side_dist_x;
	double	side_dist_y;
	double	delta_dist_x;
	double	delta_dist_y;
	double	perp_wall_dist;
	double	wall_x;
	int		step_x;
	int		step_y;
	int		hit;
	int		side;
	int		line_height;
	int		draw_start;
	int		draw_end;
}			t_ray;

double	ft_get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec + tv.tv_usec / 1000000.0);
}

int	ft_define_color(double ray_dir_x, double ray_dir_y, int side)
{
	int	color;

	if (side == 0)
		color = (ray_dir_x > 0) ? 0xFF0000 : 0x800000;
	else
		color = (ray_dir_y > 0) ? 0x00FF00 : 0x008000;
	return (color);
}

void	ft_init_ray(t_cub *data, t_ray *ray, int x)
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

void	ft_compute_step(t_cub *data, t_ray *ray)
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

void	ft_perf_dda(t_cub *data, t_ray *ray)
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

void	ft_draw_ray(t_cub *data, t_ray *ray, int x)
{
	int				pitch;
	t_img			*tex;
	int				tex_width;
	int				tex_x;
	int				tex_height;
	double			step;
	double			tex_pos;
	int				y;
	int				tex_y;
	char			*pixel;
	unsigned int	color;

	if (ray->side == 0)
		ray->perp_wall_dist = ray->side_dist_x - ray->delta_dist_x;
	else
		ray->perp_wall_dist = ray->side_dist_y - ray->delta_dist_y;
	ray->line_height = (int)data->screen_height / ray->perp_wall_dist;
	pitch = 100;
	ray->draw_start = -ray->line_height / 2 + data->screen_height / 2 + pitch;
	if (ray->draw_start < 0)
		ray->draw_start = 0;
	ray->draw_end = ray->line_height / 2 + data->screen_height / 2 + pitch;
	if (ray->draw_end >= data->screen_height)
		ray->draw_end = data->screen_height - 1;
	if (ray->side == 0)
		ray->wall_x = data->player.pos_y + ray->perp_wall_dist * ray->ray_dir_y;
	else
		ray->wall_x = data->player.pos_x + ray->perp_wall_dist * ray->ray_dir_x;
	ray->wall_x -= floor(ray->wall_x);
	if (ray->side == 0)
	{
		if (ray->ray_dir_x > 0)
			tex = &data->textures.wall_e;
		else
			tex = &data->textures.wall_w;
	}
	else
	{
		if (ray->ray_dir_y > 0)
			tex = &data->textures.wall_s;
		else
			tex = &data->textures.wall_n;
	}
	tex_width = tex->width;
	tex_x = (int)(ray->wall_x * (double)tex_width);

	if (ray->side == 0 && ray->ray_dir_x < 0)
		tex_x = tex_width - tex_x - 1;
	if (ray->side == 1 && ray->ray_dir_y > 0)
		tex_x = tex_width - tex_x - 1;

	if (tex_x < 0)
		tex_x = 0;
	else if (tex_x >= tex_width)
		tex_x = tex_width - 1;
	tex_height = tex->height;
	step = 1.0 * tex_height / ray->line_height;
	tex_pos = (ray->draw_start - pitch - data->screen_height / 2
			+ ray->line_height / 2) * step;

	y = 0;
	while (y < ray->draw_start)
	{
		ft_img_pixel_put(&data->display, x, y, data->textures.ceiling_rgb);
		y++;
	}
	y = ray->draw_start;
	while (y < ray->draw_end)
	{
		tex_y = (int)tex_pos;
		tex_pos += step;
		if (tex_y < 0)
			tex_y = 0;
		else if (tex_y >= tex_height)
			tex_y = tex_height - 1;
		if (tex->addr)
		{
			pixel = tex->addr + (tex_y * tex->line_len + tex_x * (tex->bpp
						/ 8));
			color = *(unsigned int *)pixel;
			// if (color != 0xFFF00000)
			// {
				if (ray->side == 1)
					color = (color >> 1) & 8355711;
				ft_img_pixel_put(&data->display, x, y, color);
			// }
		}
		(void)pixel;
		y++;
	}
	y = ray->draw_end;
	while (y < data->screen_height)
	{
		ft_img_pixel_put(&data->display, x, y, data->textures.floor_rgb);
		y++;
	}
}
// void ft_draw_ray(t_cub *data, t_ray *ray, int x)
// {
// 	int y;

// 	if (ray->side == 0)
// 		ray->perp_wall_dist = (ray->map_x - data->player.pos_x + (1
// - ray->step_x) / 2) / ray->ray_dir_x;
// 	else
// 		ray->perp_wall_dist = (ray->map_y - data->player.pos_y + (1
// - ray->step_y) / 2) / ray->ray_dir_y;
// 	ray->line_height = (int)(data->screen_height / ray->perp_wall_dist);
// 	ray->draw_start = -ray->line_height / 2 + data->screen_height / 2;
// 	if (ray->draw_start < 0)
// 		ray->draw_start = 0;
// 	ray->draw_end = ray->line_height / 2 + data->screen_height / 2;
// 	if (ray->draw_end >= data->screen_height)
// 		ray->draw_end = data->screen_height - 1;
// 	y = ray->draw_start;
// 	while (y < ray->draw_end)
// 	{
// 		ft_img_pixel_put(&data->display, x, y, ft_define_color(ray->ray_dir_x,
// ray->ray_dir_y, ray->side));
// 		y++;
// 	}
// }

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
	// ft_img_fill(&data->display, data->display.height, data->display.width,
	// 	data->textures.floor_rgb);
	// ft_img_fill(&data->display, data->display.height / 2, data->display.width,
	// 	data->textures.ceiling_rgb);
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
