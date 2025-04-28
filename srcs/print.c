/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 11:09:06 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/28 17:32:17 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

void	print_header(t_traceroute *traceroute)
{
	printf("traceroute to %s (%s), %d hops max\n",
		traceroute->hostname,
		traceroute->ip,
		DEFAULT_HOPS);
}

void	print_stats(t_traceroute *traceroute, int probe, int received_bytes[DEFAULT_PROBES], struct sockaddr_in *received_addr[DEFAULT_PROBES])
{
	static struct in_addr *last_addr	= NULL;
	char *ip							= inet_ntoa(received_addr[probe]->sin_addr);
	
	// DNS resolution
	// char *host					= NULL;
	// struct hostent *recv_host	= gethostbyaddr(&received_addr[probe], sizeof(received_addr[probe]), AF_INET);
	// if (recv_host)
	// 	host = recv_host->h_name;
	// else
	// 	host = inet_ntoa(received_addr[probe]->sin_addr);
	
	// Show ip address
	if (!last_addr && received_bytes[probe] >= 0)
	{
		printf("  %s", ip);
		last_addr = &received_addr[probe]->sin_addr;
	}
	else if (last_addr && last_addr->s_addr != received_addr[probe]->sin_addr.s_addr)
	{
		printf("  %s", ip);
		last_addr = &received_addr[probe]->sin_addr;
	}

	// Show RTT (round trip time) or * if no response
	if (received_bytes[probe] < 0)
		printf("  *");
	if (received_bytes[probe] > 0)
	{	
		double elapsed =  (traceroute->end.tv_sec - traceroute->start.tv_sec) * 1000.0 +
							(traceroute->end.tv_usec - traceroute->start.tv_usec) / 1000.0;
		printf("  %.3f ms", elapsed);
	}

	if (probe == DEFAULT_PROBES - 1)
	{
		printf("\n");
		last_addr = NULL;
	}
}
