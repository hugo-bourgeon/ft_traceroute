/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:05:00 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/28 20:05:52 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

void	parsing(int argc, char **argv, t_traceroute *traceroute)
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
		free_all(EXIT_FAILURE, traceroute);
	}

	traceroute->hostname = strdup(argv[1]);
	if (!traceroute->hostname)
	{
		fprintf(stderr, "strdup error\n");
		free_all(EXIT_FAILURE, traceroute);
	}
}
