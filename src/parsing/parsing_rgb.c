/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_rgb.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 12:57:35 by alamjada          #+#    #+#             */
/*   Updated: 2026/04/04 16:45:26 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FT_ASSIGN_WALL - ASSIGNS A TEXTURE PATH TO THE MATCHING WALL FIELD
int	ft_assign_wall(t_cub *data, char **dst, char *path, char **arr)
{
	if (arr[2])
		ft_exit(data, ERRN_PARSING, ERR_MSG_INVALID_TEXT, arr[2]);
	if (!arr[1])
		ft_exit(data, ERRN_PARSING, ERR_MSG_INVALID_TEXT, arr[0]);
	*dst = ft_strdup_gc(path, &data->gc_global);
	if (!dst)
		ft_exit(data, ERRN_MALLOC, ERR_MSG_PARSING, ERR_MSG_MALLOC);
	return (SUCCESS);
}

// FT_ASSIGN_F_C - ASSIGNS A FLOOR OR CEILING RGB COLOR VALUE
int	ft_assign_f_c(t_cub *data, int *dst, char **arr, t_background part)
{
	if (*dst != -1 && part == FLOOR)
		ft_exit(data, ERRN_TEXTURES, ERR_MSG_SET_COLOR, "FLOOR" );
	if (*dst != -1 && part == CEILING)
		ft_exit(data, ERRN_TEXTURES, ERR_MSG_SET_COLOR, "CEILING" );
	if (ft_rgb(data, arr, part) != SUCCESS)
	{
		*dst = -1;
		return (FAILURE);
	}
	return (SUCCESS);
}

// FT_RGB_CONVERT - CONVERTS AN RGB STRING COMPONENT TO AN INTEGER VALUE
int	ft_rgb_convert(char *str, int *nb)
{
	int	i;

	*nb = 0;
	i = -1;
	if (ft_strlen(str) > 3 || !ft_is_only(str, ft_isdigit))
	{
		*nb = -1;
		return (ft_error(ERR_MSG_INVALID_RGB, str, ERRN_PARSING));
	}
	while (str[++i])
		*nb = *nb * 10 + str[i] - '0';
	if (*nb > 255)
	{
		*nb = -1;
		return (ft_error(ERR_MSG_INVALID_RGB, str, ERRN_PARSING));
	}
	return (SUCCESS);
}

// AIDE A CREER RGB VALUE
static void	ft_rgb_affect(t_cub *data, int *rgb, t_background part)
{
	int	ret;

	if (rgb[0] == -1 || rgb[1] == -1 || rgb[2] == -1)
		ret = -1;
	else
		ret = 255 << 24 | rgb[0] << 16 | rgb[1] << 8 | rgb[2];
	if (part == FLOOR)
		data->textures.floor_rgb = ret;
	if (part == CEILING)
		data->textures.ceiling_rgb = ret;
}

// FT_RGB - PARSES AN RGB COLOR LINE AND STORES IT IN DATA
int	ft_rgb(t_cub *data, char **arr, t_background part)
{
	int		rgb[3];
	int		index;
	int		i;
	int		j;
	char	**split;

	i = -1;
	index = 0;
	while (arr[++i])
	{
		j = 0;
		if (i == 0)
			arr[i] = ft_strtrim_gc(arr[i], "FC", &data->gc_global);
		if (!arr[i][0])
			continue ;
		split = ft_split_charset_gc(arr[i], ",", &data->gc_global);
		if (index >= 3)
			ft_exit(data, ERRN_PARSING, ERR_MSG_INVALID_RGB, arr[i]);
		if (ft_rgb_convert(split[j++], &rgb[index++]) != SUCCESS)
			return (FAILURE);
		split = NULL;
	}
	ft_rgb_affect(data, rgb, part);
	return (SUCCESS);
}
