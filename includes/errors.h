/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/04/04 13:54:11 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_H
# define ERRORS_H

typedef enum e_err
{
	ERRN_NEGATIVE_FAIL = -1,
	ERRN_NONE,
	ERRN_EMPTY_FILE,
	ERRN_MALLOC,
	ERRN_LOAD,
	ERRN_OPEN,
	ERRN_ARGS,
	ERRN_TEXTURES,
	ERRN_PARSING,
	ERRN_WALL,
	ERRN_UNKNOWN
}		t_err;

/* ============== ERROR MESSAGES ============================ */

# define ERR_MSG_MALLOC "memory allocation failed"

/* ========= ERRORS_PARSING ============ */

# define ERR_MSG_ARGS "wrong number of arguments"
# define ERR_MSG_ARGC "takes only : ./cub3d ./path_to_map"
# define ERR_MSG_FORMAT "file must end in .cub"
# define ERR_MSG_INVALID_ID "Identifier not declared right"
# define ERR_MSG_INVALID_CHAR "Invalid char found on the map"
# define ERR_MSG_INVALID_TEXT "Invalid texture declaration: "
# define ERR_MSG_INVALID_RGB "Invalid RGB value"
# define ERR_MSG_MISS_CEILING "Missing CEILING informations"
# define ERR_MSG_MISS_FLOOR "Missing FLOOR informations"
# define ERR_MSG_MISS_NORTH "Missing NORTH WALL informations"
# define ERR_MSG_MISS_SOUTH "Missing SOUTH WALL informations"
# define ERR_MSG_MISS_WEST "Missing WEST WALL informations or wrong declaration"
# define ERR_MSG_MISS_EAST "Missing EAST WALL informations or wrong declaration"
# define ERR_MSG_MAX_RGB "Too many values for RGB"
# define ERR_MSG_PLAYER_COUNT "Too much player located on the map"
# define ERR_MSG_NO_PLAYER "No player located on the map"
# define ERR_MSG_SET_COLOR "Color is already set"
# define ERR_MSG_WALLS "Map isn't closed by walls"
# define ERR_MSG_PARSING "Error parsing"
# define ERR_MSG_TEXTURES "Couldn't load all textures"
# define ERR_MSG_XPM "Couldn't load this .xpm file: "
# define ERR_MSG_OPEN "Cannot open file"
# define ERR_MSG_EMPTY "Empty file"
# define ERR_MSG_LOADING "Error while loading game"
# define ERR_MSG_MLX "Mlx didn't initiate properly"
# define ERR_FAIL_MLX "Something went wrong with MLX"

/* ============== ERROR FUNCTIONS ========================== */
/* src/utils/error.c */
int		ft_error(char *context, char *detail, int error);
void	ft_exit(t_cub *data, int error, char *context, char *detail);
int		check_args(int argc, int expected, char *usage);

/* ============== MEMORY HELPERS =========================== */
/* src/utils/memory.c */
void	*safe_malloc(size_t size);
void	free_split(char **tab);
void	free_data(t_cub *data);

#endif
