/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_textures.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 19:19:24 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/30 12:11:57 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int ft_rgb_convert(char *str, int *nb)
{
	int i;

	*nb = 0;
	i = -1;
	if (ft_strlen(str) > 3 || !ft_is_only(str, ft_isdigit))
	{
		*nb = -1;
		return (ft_error(ERR_MSG_INVALID_RGB, str, ERRN_PARSING));
	}
	while (str[++i])
		*nb = *nb * 10 + str[i] - '0';
	return (SUCCESS);
}

// AIDE A CREER RGB VALUE
static void ft_rgb_affect(t_cub *data, int *rgb, t_background part)
{
	int ret;

	ret = 255 << 24 | rgb[0] << 16 | rgb[1] << 8 | rgb[2];
	if (part == FLOOR)
		data->textures.floor_rgb = ret;
	if (part == CEILING)
		data->textures.ceiling_rgb = ret;
}

// TODO : A REFACTORISER POUR LA NORME, TROUVER UN MOYEN PLUN FUN
// POUR CONVERTIR EN COULEUR
int ft_rgb(t_cub *data, char **arr, t_background part)
{
	int rgb[3];
	int ret;
	int i;
	int j;
	char **split;

	i = -1;
	ret = 0;
	rgb[0] = -1;
	rgb[1] = -1;
	rgb[2] = -1;
	while (arr[++i])
	{
		j = -1;
		split = ft_split_charset_gc(arr[i], ", \t", &data->gc_tmp);
		if (!split)
			return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
		while (split[++j])
		{
			if (i == 0 && j == 0)
				split[j] = ft_strtrim_gc(split[j], "FC", &data->gc_tmp);
			if (!ft_is_only(split[j], ft_isdigit))
				return (ft_error(ERR_MSG_INVALID_RGB, split[j], ERRN_PARSING));
			if (split[j][0] && rgb[0] == -1)
			{
				if (ft_rgb_convert(split[j], &rgb[0]) != SUCCESS)
					return (FAILURE);
			}
			else if (split[j][0] && rgb[1] == -1)
			{
				if (ft_rgb_convert(split[j], &rgb[1]) != SUCCESS)
					return (FAILURE);
			}
			else if (split[j][0] && rgb[2] == -1)
				if (ft_rgb_convert(split[j], &rgb[2]) != SUCCESS)
					return (FAILURE);
		}
	}
	ft_rgb_affect(data, rgb, part);
	return (SUCCESS);
}

/*
** FT_TEXTURES_DETECT - CHECKS IF ID IS A VALID TEXTURE/COLOR IDENTIFIER
** RETURNS SUCCESS IF ID IS NO, SO, EA, WE, F OR C, FAILURE OTHERWISE
*/
static int ft_textures_detect(char *id)
{
	if (ft_strncmp(id, "NO", 2) != 0 && ft_strncmp(id, "SO", 2) != 0 &&
		ft_strncmp(id, "EA", 2) != 0 && ft_strncmp(id, "WE", 2) != 0 &&
		ft_strncmp(id, "F", 1) != 0 && ft_strncmp(id, "C", 1) != 0)
			return (FAILURE);
	return (SUCCESS);
}

/*
** FT_TEXTURE_DISPATCH - ASSIGNS A TEXTURE PATH TO THE MATCHING FIELD
** MATCHES ID (NO, SO, WE, EA, F, C) AND STORES PATH IN DATA->TEXTURES
*/

static int ft_texture_dispatch(t_cub *data, char **arr, char *id, char *path)
{
	if (ft_strncmp(id, "NO", 2) == 0)
	{
		data->textures.north = ft_strdup_gc(path, &data->gc_global);
		if (!data->textures.north)
			return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	}
	if (ft_strncmp(id, "SO", 2) == 0)
	{
		data->textures.south = ft_strdup_gc(path, &data->gc_global);
		if (!data->textures.south)
			return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	}
	if (ft_strncmp(id, "WE", 2) == 0)
	{
		data->textures.west = ft_strdup_gc(path, &data->gc_global);
		if (!data->textures.west)
			return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	}
	if (ft_strncmp(id, "EA", 2) == 0)
	{
		data->textures.east = ft_strdup_gc(path, &data->gc_global);
		if (!data->textures.east)
			return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	}
	if (ft_strncmp(id, "F", 1) == 0)
	{
		if (data->textures.floor_rgb != 0)
			return (ft_error(ERR_MSG_SET_COLOR, "FLOOR", ERRN_TEXTURES));
		if (ft_rgb(data, arr, FLOOR) != SUCCESS)
		{
			data->textures.floor_rgb = -1;
			return (FAILURE);
		}
	}
	if (ft_strncmp(id, "C", 1) == 0)
	{
		if (data->textures.ceiling_rgb != 0)
			return (ft_error(ERR_MSG_SET_COLOR, "CEILING", ERRN_TEXTURES));
		if (ft_rgb(data, arr, CEILING) != SUCCESS)
		{
			data->textures.ceiling_rgb = -1;
			return (FAILURE);
		}
	}
	return (SUCCESS);
}

/*
** FT_TEXTURES_FILL - SPLITS A LINE AND FILLS TEXTURE DATA
** RETURNS SUCCESS IF LINE IS A VALID TEXTURE, FAILURE IF NOT
*/
static int ft_textures_fill(t_cub *data, char *line)
{
	char **arr;

	arr = ft_split_charset_gc((const char *)line, " \t\n", &data->gc_tmp);
	if (!arr)
		return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));

	if (!arr[0] || ft_textures_detect(arr[0]))
		return (FAILURE);

	// AFFECTS THE TEXTURES TO THE DESIRED IMAGE
	if (ft_texture_dispatch(data, arr, arr[0], arr[1]) != SUCCESS)
		return (ERRN_TEXTURES);
	return (SUCCESS);
}

int ft_textures_complete(t_textures *textures)
{
	if (textures->ceiling_rgb == -1 || textures->floor_rgb == -1)
		return (FAILURE);
	if (textures->north == NULL || textures->south == NULL)
		return (FAILURE);
	if (textures->east == NULL || textures->west == NULL)
		return (FAILURE);
	return (SUCCESS);
}

/*
** FT_TEXTURES_PARSING - ITERATES FILE LINES TO PARSE TEXTURES AND COLORS
** STOPS WHEN IT HITS THE FIRST MAP LINE AND SETS DATA->INDEX_MAP_START
*/
int ft_textures_parsing(t_cub *data)
{
	int i;
	int ret;

	i = -1;
	while (data->file[++i])
	{
		ret = ft_textures_fill(data, data->file[i]);
		if (ret != SUCCESS && ret != FAILURE)
			return (ret);
		if (ret == FAILURE && !ft_is_only(data->file[i], ft_isspace))
		{
			if (ft_textures_complete(&data->textures) != SUCCESS)
				return (ft_error(ERR_MSG_LOADING, ERR_MSG_TEXTURES, ERRN_LOAD));
			data->map.index_map_start = i;
			return (SUCCESS);
		}
	}
	if (ft_textures_complete(&data->textures) != SUCCESS)
		return (ft_error(ERR_MSG_LOADING, ERR_MSG_TEXTURES, ERRN_LOAD));
	return (SUCCESS);
}
