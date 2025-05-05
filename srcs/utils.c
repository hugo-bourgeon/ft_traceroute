/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:07:42 by hubourge          #+#    #+#             */
/*   Updated: 2025/05/04 23:49:04 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

void	free_all(int exit_code, t_traceroute *traceroute)
{
	if (traceroute)
	{
		if (traceroute->icmp_sockfd > 0)
			close(traceroute->icmp_sockfd);
		if (traceroute->send_sockfd > 0)
			close(traceroute->send_sockfd);
		if (traceroute->recv_sockfd > 0)
			close(traceroute->recv_sockfd);
		if (traceroute->hostname)
			free(traceroute->hostname);
		if (traceroute->ip)
			free(traceroute->ip);
		if (traceroute->udp_dest_result)
			freeaddrinfo(traceroute->udp_dest_result);
		if (traceroute->icmp_dest_result)
			freeaddrinfo(traceroute->icmp_dest_result);
		if (traceroute->flag)
			free(traceroute->flag);
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

void	handle_sigint(int sig)
{
	(void)sig;
	g_stop_code = STOP;
}

void	check_sigint(t_traceroute *traceroute)
{
	if (g_stop_code == STOP)
		free_all(EXIT_SUCCESS, traceroute);
}