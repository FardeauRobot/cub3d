/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 11:59:48 by fardeau           #+#    #+#             */
/*   Updated: 2026/04/03 19:11:34 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FT_PLAYER_SET - SETS THE PLAYER POSITION AND DIRECTION FROM ORIENTATION
void	ft_player_set(t_player *player, int x, int y, char orient)
{
	player->pos_y = (double)y + 0.5;
	player->pos_x = (double)x + 0.5;
	if (orient == 'N')
	{
		player->dir_x = 0;
		player->dir_y = -1;
	}
	if (orient == 'S')
	{
		player->dir_x = 0;
		player->dir_y = 1;
	}
	if (orient == 'E')
	{
		player->dir_x = 1;
		player->dir_y = 0;
	}
	if (orient == 'W')
	{
		player->dir_x = -1;
		player->dir_y = 0;
	}
}

// FT_FORMAT_CHECK - VALIDATES THAT THE FILE EXTENSION IS .CUB
int	ft_format_check(char *filepath)
{
	int	len_path;

	len_path = ft_strlen(filepath);
	if (ft_strncmp(&filepath[len_path - 4], ".cub", 4))
		return (FAILURE);
	return (SUCCESS);
}

// FT_FILE_DUP - DUPLICATES THE LINKED LIST OF LINES INTO DATA->FILE ARRAY
static void	ft_file_dup(t_cub *data, t_list *lst)
{
	int	i;

	i = 0;
	while (lst)
	{
		data->file[i++] = ft_strdup_gc(lst->content, &data->gc_global);
		lst = lst->next;
	}
}

// FT_FILE_STORE - READS THE .CUB FILE AND STORES EACH LINE IN DATA->FILE
int	ft_file_store(t_cub *data)
{
	char	*line;
	int		i;
	t_list	*node;
	t_list	*lst;

	i = 0;
	lst = NULL;
	line = get_next_line(data->fd_map);
	while (line)
	{
		if (ft_gc_add_node(&data->gc_tmp, line))
			ft_exit(data, ERRN_MALLOC, ERR_MSG_PARSING, ERR_MSG_MALLOC);
		node = ft_lstnew_gc(line, &data->gc_tmp);
		if (!node)
			ft_exit(data, ERRN_MALLOC, ERR_MSG_PARSING, ERR_MSG_MALLOC);
		ft_lstadd_back(&lst, node);
		i++;
		line = get_next_line(data->fd_map);
	}
	data->file = ft_calloc_gc(i + 1, sizeof(char *), &data->gc_global);
	if (!data->file)
		ft_exit(data, ERRN_MALLOC, ERR_MSG_PARSING, ERR_MSG_MALLOC);
	ft_file_dup(data, lst);
	return (SUCCESS);
}
