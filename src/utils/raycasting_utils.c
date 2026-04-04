/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 17:47:49 by alamjada          #+#    #+#             */
/*   Updated: 2026/04/03 19:10:08 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FT_GET_X_TEX - COMPUTES THE X COORDINATE ON THE WALL TEXTURE
void	ft_get_x_tex(t_ray *ray, t_draw *dw)
{
	dw->tex_width = dw->tex->width;
	dw->tex_x = (int)(ray->wall_x * (double)dw->tex_width);
	if (ray->side == 0 && ray->ray_dir_x < 0)
		dw->tex_x = dw->tex_width - dw->tex_x - 1;
	if (ray->side == 1 && ray->ray_dir_y > 0)
		dw->tex_x = dw->tex_width - dw->tex_x - 1;
	if (dw->tex_x < 0)
		dw->tex_x = 0;
	else if (dw->tex_x >= dw->tex_width)
		dw->tex_x = dw->tex_width - 1;
}

// FT_GET_Y_TEX - COMPUTES THE Y COORDINATE AND STEP FOR THE WALL TEXTURE
void	ft_get_y_tex(t_cub *data, t_ray *ray, t_draw *dw)
{
	dw->tex_height = dw->tex->height;
	dw->step = 1.0 * dw->tex_height / ray->line_height;
	dw->tex_pos = (ray->draw_start - dw->pitch - data->screen_height / 2
			+ ray->line_height / 2) * dw->step;
}

// FT_DRAW_CEILING - DRAWS THE CEILING PIXELS ABOVE THE WALL STRIPE
void	ft_draw_ceiling(t_cub *data, t_ray *ray, int x)
{
	int	y;

	y = 0;
	while (y < ray->draw_start)
	{
		ft_img_pixel_put(&data->display, x, y, data->textures.ceiling_rgb);
		y++;
	}
}

// FT_DRAW_WALL - DRAWS THE TEXTURED WALL PIXELS FOR A SCREEN COLUMN
void	ft_draw_wall(t_cub *data, t_ray *ray, t_draw *dw, int x)
{
	int	y;

	y = ray->draw_start;
	while (y < ray->draw_end)
	{
		dw->tex_y = (int)dw->tex_pos;
		dw->tex_pos += dw->step;
		if (dw->tex_y < 0)
			dw->tex_y = 0;
		else if (dw->tex_y >= dw->tex_height)
			dw->tex_y = dw->tex_height - 1;
		if (dw->tex->addr)
		{
			dw->pixel = dw->tex->addr + (dw->tex_y * dw->tex->line_len
					+ dw->tex_x * (dw->tex->bpp / 8));
			dw->color = *(unsigned int *)dw->pixel;
			if (ray->side == 1)
				dw->color = (dw->color >> 1) & 8355711;
			ft_img_pixel_put(&data->display, x, y, dw->color);
		}
		y++;
	}
}

// FT_DRAW_FLOOR - DRAWS THE FLOOR PIXELS BELOW THE WALL STRIPE
void	ft_draw_floor(t_cub *data, t_ray *ray, int x)
{
	int	y;

	y = ray->draw_end;
	while (y < data->screen_height)
	{
		ft_img_pixel_put(&data->display, x, y, data->textures.floor_rgb);
		y++;
	}
}
