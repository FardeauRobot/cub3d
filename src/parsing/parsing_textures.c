/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_textures.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 19:19:24 by fardeau           #+#    #+#             */
/*   Updated: 2026/04/04 13:32:39 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FT_TEXTURES_DETECT - CHECKS IF ID IS A VALID TEXTURE OR COLOR IDENTIFIER
static int	ft_textures_detect(char *id)
{
	if (ft_strncmp(id, "NO", 2) != 0 && ft_strncmp(id, "SO", 2) != 0
		&& ft_strncmp(id, "EA", 2) != 0 && ft_strncmp(id, "WE", 2) != 0
		&& ft_strncmp(id, "F", 1) != 0 && ft_strncmp(id, "C", 1) != 0)
		return (FAILURE);
	return (SUCCESS);
}

// FT_TEXTURE_DISPATCH - DISPATCHES A TEXTURE LINE TO THE MATCHING FIELD
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

// FT_TEXTURES_FILL - SPLITS A LINE AND FILLS THE CORRESPONDING TEXTURE DATA
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

// FT_TEXTURES_COMPLETE - CHECKS THAT ALL REQUIRED TEXTURES HAVE BEEN SET
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

// FT_TEXTURES_PARSING - ITERATES FILE LINES TO PARSE TEXTURES AND COLORS
int	ft_textures_parsing(t_cub *data)
{
	int	i;
	int	ret;

	i = -1;
	while (data->file[++i])
	{
		ret = ft_textures_fill(data, data->file[i]);
		if ((ret == FAILURE || ret == ERRN_TEXTURES)
			&& !ft_is_only(data->file[i], ft_isspace))
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
