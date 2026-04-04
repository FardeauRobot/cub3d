/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_rgb.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 12:57:35 by alamjada          #+#    #+#             */
/*   Updated: 2026/04/04 13:32:07 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** FT_TEXTURE_DISPATCH - ASSIGNS A TEXTURE PATH TO THE MATCHING FIELD
** MATCHES ID (NO, SO, WE, EA, F, C) AND STORES PATH IN DATA->TEXTURES
*/
int	ft_assign_wall(t_cub *data, char **dst, char *path, char **arr)
{
	if (arr[2])
		return (ft_error("Invalid texture", arr[2], ERRN_PARSING));
	*dst = ft_strdup_gc(path, &data->gc_global);
	if (!dst)
		return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	return (SUCCESS);
}

int	ft_assign_f_c(t_cub *data, int *dst, char **arr, t_background part)
{
	if (*dst != 0)
		return (ft_error(ERR_MSG_SET_COLOR, (char *)part, ERRN_TEXTURES));
	if (ft_rgb(data, arr, part) != SUCCESS)
	{
		*dst = -1;
		return (FAILURE);
	}
	return (SUCCESS);
}

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

// TODO : A REFACTORISER POUR LA NORME, TROUVER UN MOYEN PLUN FUN
// POUR CONVERTIR EN COULEUR
int	ft_rgb(t_cub *data, char **arr, t_background part)
{
	int	rgb[3];
	int	index;
	int	i;

	i = -1;
	index = 0;
	while (arr[++i])
	{
		if (i == 0)
			arr[i] = ft_strtrim_gc(arr[i], "FC", &data->gc_global);
		if (!arr[i][0])
			continue ;
		if (index >= 3)
			return (ft_error(ERR_MSG_INVALID_RGB, arr[i], ERRN_PARSING));
		if (ft_rgb_convert(arr[i], &rgb[index++]) != SUCCESS)
			return (FAILURE);
	}
	ft_rgb_affect(data, rgb, part);
	return (SUCCESS);
}
