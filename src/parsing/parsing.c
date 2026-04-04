/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 11:03:04 by fardeau           #+#    #+#             */
/*   Updated: 2026/04/04 12:54:30 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// TODO : HOW DO WE HANDLE THE CASE WHERE A TEXTURE INFO IS AFTER THE MAP
/*
** FT_PARSING - MAIN PARSING ENTRY POINT
** VALIDATES ARGS, FILE FORMAT, STORES FILE, PARSES TEXTURES AND MAP
*/
int	ft_parsing(t_cub *data, char **argv, int argc)
{
	if (argc != 2)
		ft_exit(data, ERRN_PARSING, ERR_MSG_ARGS, ERR_MSG_ARGC);
	if (ft_format_check(argv[1]) != SUCCESS)
		ft_exit(data, ERRN_PARSING, ERR_MSG_OPEN, ERR_MSG_FORMAT);
	data->fd_map = open(argv[1], O_RDONLY);
	if (data->fd_map == -1)
		ft_exit(data, ERRN_OPEN, ERR_MSG_OPEN, argv[1]);
	if (ft_file_store(data) != SUCCESS)
		ft_exit(data, ERRN_PARSING, NULL, NULL);
	if (ft_textures_parsing(data) != SUCCESS)
		ft_exit(data, ERRN_PARSING, NULL, NULL);
	ft_map_fill(data);
	ft_map_check(data);
	if (data->player.pos_x == 0 || data->player.pos_y == 0)
		ft_exit(data, ERRN_PARSING, ERR_MSG_PARSING, ERR_MSG_NO_PLAYER);
	ft_gc_free_all(&data->gc_tmp);
	return (SUCCESS);
}
