/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:04:53 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/25 19:17:14 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

void	init(t_traceroute **traceroute)
{
	*traceroute = malloc(sizeof(t_traceroute));
	if (!(*traceroute))
	{
		perror("malloc");
		free_all(EXIT_FAILURE, *traceroute);
	}
	(*traceroute)->hostname = NULL;
}
