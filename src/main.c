/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by author            #+#    #+#             */
/*   Updated: 2026/03/26 12:32:23 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FUNCTION USED TO INITIALIZE THE MAIN STRUCTURE AND START THE PROGRAM
int	main(int argc, char **argv)
{
	t_cub	data;

	ft_bzero(&data, sizeof(t_cub));
	ft_parsing(&data, argv, argc);
	ft_game(&data);
	return (EXIT_SUCCESS);
}
