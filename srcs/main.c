/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 15:36:01 by hubourge          #+#    #+#             */
/*   Updated: 2025/05/04 23:46:13 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

int		g_stop_code = PROCESS;

int main(int argc, char **argv)
{
	t_traceroute	*traceroute;
	
	init(&traceroute);
	parsing(argc, argv, traceroute);
	init_dest(traceroute);
	init_socket(traceroute);
	ft_traceroute(traceroute);

	free_all(NOT_EXIT, traceroute);
	return (0);
}
