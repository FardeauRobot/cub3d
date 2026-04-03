/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_textures.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 19:19:24 by fardeau           #+#    #+#             */
/*   Updated: 2026/04/03 14:56:11 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	ft_rgb_convert(char *str, int *nb)
{
	int	i;

	*nb = 0;
	i = -1;
	printf("str = %s\n", str);
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
	int		rgb[3];
	int 	index;
	int		i;

	i = -1;
	index = 0;
	ft_matrix_print(arr);
	while (arr[++i])
	{
		if (i == 0)
			arr[i] = ft_strtrim_gc(arr[i], "FC", &data->gc_global);
		if (!arr[i][0])
			continue;
		if (index >= 3)
			return (ft_error(ERR_MSG_INVALID_RGB, arr[i], ERRN_PARSING));
		if (ft_rgb_convert(arr[i], &rgb[index++]) != SUCCESS)
			return (FAILURE);
	}
	ft_rgb_affect(data, rgb, part);
	return (SUCCESS);
}

/*
** FT_TEXTURES_DETECT - CHECKS IF ID IS A VALID TEXTURE/COLOR IDENTIFIER
** RETURNS SUCCESS IF ID IS NO, SO, EA, WE, F OR C, FAILURE OTHERWISE
*/
static int	ft_textures_detect(char *id)
{
	if (ft_strncmp(id, "NO", 2) != 0 && ft_strncmp(id, "SO", 2) != 0
		&& ft_strncmp(id, "EA", 2) != 0 && ft_strncmp(id, "WE", 2) != 0
		&& ft_strncmp(id, "F", 1) != 0 && ft_strncmp(id, "C", 1) != 0)
		return (FAILURE);
	return (SUCCESS);
}

/*
** FT_TEXTURE_DISPATCH - ASSIGNS A TEXTURE PATH TO THE MATCHING FIELD
** MATCHES ID (NO, SO, WE, EA, F, C) AND STORES PATH IN DATA->TEXTURES
*/

static int ft_assign_wall(t_cub *data, char **dst, char *path, char **arr)
{
	if (arr[2])
		return (ft_error("Invalide tex", arr[2], ERRN_PARSING));
	*dst = ft_strdup_gc(path, &data->gc_global);
	if (!dst)
		return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	return (SUCCESS);
}

static int ft_assign_f_c(t_cub *data, int *dst, char **arr, t_background part)
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

static int	ft_texture_dispatch(t_cub *data, char **arr, char *id, char *path)
{
	if (ft_strncmp(id, "NO", 2) == 0)
		return (ft_assign_wall(data, &data->textures.north, path, arr));
	if (ft_strncmp(id, "SO", 2) == 0)
		return (ft_assign_wall(data, &data->textures.south, path, arr));
	if (ft_strncmp(id, "WE", 2) == 0)
		return (ft_assign_wall(data, &data->textures.west, path, arr));
	if (ft_strncmp(id, "EA", 2) == 0)
		return (ft_assign_wall(data, &data->textures.east, path, arr));
	if (ft_strncmp(id, "F", 1) == 0)
		return (ft_assign_f_c(data, &data->textures.floor_rgb, arr, FLOOR));
	if (ft_strncmp(id, "C", 1) == 0)
		return (ft_assign_f_c(data, &data->textures.ceiling_rgb, arr, CEILING));
	return (FAILURE);
}

/*
** FT_TEXTURES_FILL - SPLITS A LINE AND FILLS TEXTURE DATA
** RETURNS SUCCESS IF LINE IS A VALID TEXTURE, FAILURE IF NOT
*/
static int	ft_textures_fill(t_cub *data, char *line)
{
	char	**arr;

	arr = ft_split_charset_gc((const char *)line, ", \t\n", &data->gc_tmp);
	if (!arr)
		return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	if (!arr[0] || ft_textures_detect(arr[0]) == FAILURE)
		return (FAILURE);
	if (ft_texture_dispatch(data, arr, arr[0], arr[1]) != SUCCESS)
		return (ERRN_TEXTURES);
	return (SUCCESS);
}

// TODO : COMPRENDRE POURQUOI PAS D'INTERRUPTION SI MAUVAIS CHAR
int	ft_textures_complete(t_textures *textures)
{
	if (textures->ceiling_rgb == -1)
		return (ft_error(ERR_MSG_TEXTURES, ERR_MSG_MISS_CEILING, ERRN_PARSING));
	if (textures->floor_rgb == -1)
		return (ft_error(ERR_MSG_TEXTURES, ERR_MSG_MISS_FLOOR, ERRN_PARSING));
	if (textures->north == NULL)
		return (ft_error(ERR_MSG_TEXTURES, ERR_MSG_MISS_NORTH, ERRN_PARSING));
	if (textures->south == NULL)
		return (ft_error(ERR_MSG_TEXTURES, ERR_MSG_MISS_SOUTH, ERRN_PARSING));
	if (textures->east == NULL)
		return (ft_error(ERR_MSG_TEXTURES, ERR_MSG_MISS_EAST, ERRN_PARSING));
	if (textures->west == NULL)
		return (ft_error(ERR_MSG_TEXTURES, ERR_MSG_MISS_WEST, ERRN_PARSING));
	return (SUCCESS);
}

/*
** FT_TEXTURES_PARSING - ITERATES FILE LINES TO PARSE TEXTURES AND COLORS
** STOPS WHEN IT HITS THE FIRST MAP LINE AND SETS DATA->INDEX_MAP_START
*/
int	ft_textures_parsing(t_cub *data)
{
	int	i;
	int	ret;

	i = -1;
	while (data->file[++i])
	{
		ret = ft_textures_fill(data, data->file[i]);
		// if (ret != SUCCESS && ret != FAILURE)
		// 	break ;
		if ((ret == FAILURE  || ret == ERRN_TEXTURES) && !ft_is_only(data->file[i], ft_isspace))
		{
			ft_cub_print(data);
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
