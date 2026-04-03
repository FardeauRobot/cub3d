/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 08:37:28 by tibras            #+#    #+#             */
/*   Updated: 2026/04/03 19:15:08 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FUNCTION USED TO WRITE ONE PIXEL AT THE GIVEN POSITION INSIDE AN IMAGE
void	ft_img_pixel_put(t_img *img, int x, int y, int color)
{
	char	*dst;

	if (!img || !img->addr)
		return ;
	if (x < 0 || y < 0)
		return ;
	if (x >= img->width || y >= img->height)
		return ;
	if (color == 0x000000)
		return ;
	dst = img->addr + (y * img->line_len + x * (img->bpp / 8));
	*(unsigned int *)dst = color;
}

// FUNCTION USED TO READ ONE PIXEL COLOR FROM AN IMAGE
int	ft_img_pixel_get(t_img *img, int x, int y)
{
	char	*src;

	if (!img || !img->addr)
		return (0);
	if (x < 0 || y < 0)
		return (0);
	if (x >= img->width || y >= img->height)
		return (0);
	src = img->addr + (y * img->line_len + x * (img->bpp / 8));
	return (*(unsigned int *)src);
}

// FUNCTION USED TO COPY A SOURCE IMAGE ONTO A DESTINATION IMAGE AT (dst_x,
//	dst_y)
void	ft_img_to_img(t_img *dst, t_img *src, int dst_x, int dst_y)
{
	int	x;
	int	y;

	y = -1;
	while (++y < src->height)
	{
		x = -1;
		while (++x < src->width)
			ft_img_pixel_put(dst, dst_x + x, dst_y + y, ft_img_pixel_get(src, x,
					y));
	}
}

// FUNCTION USED TO FILL A WHOLE IMAGE WITH A SINGLE COLOR
void	ft_img_fill(t_img *img, int height, int width, int color)
{
	int	x;
	int	y;

	y = -1;
	while (++y < height)
	{
		x = -1;
		while (++x < width)
			ft_img_pixel_put(img, x, y, color);
	}
}
