/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:07:42 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/25 19:16:43 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

void	free_all(int exit_code, t_traceroute *traceroute)
{
	if (traceroute)
	{
		if (traceroute->hostname)
			free(traceroute->hostname);
		free(traceroute);
	}
	if (exit_code == EXIT_FAILURE || exit_code == EXIT_SUCCESS)
		exit(exit_code);
}

unsigned short	checksum(void *packet, int len)
{
	unsigned short	*buffer	= packet;
	unsigned int	sum		= 0;
	unsigned short	result;
	
	// Sum all 16 bits words 
	// ex: 0800 + 0000 + 0001 + FFFF
	for (sum = 0; len > 1; len -= 2)
		sum += *(buffer)++;
	
	// If there is a byte left, add it
	if (len == 1)
		sum += *(unsigned char*)buffer;

	// Add overflow bit
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	
	// one's complement
	result = ~sum;
	return (result);
}
