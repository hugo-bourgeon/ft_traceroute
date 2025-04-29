/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 11:09:06 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/29 14:46:12 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

void	print_header(t_traceroute *traceroute)
{
	printf("traceroute to %s (%s), %d hops max\n",
		traceroute->hostname,
		traceroute->ip,
		traceroute->flag->m);
}

void	print_stats(t_traceroute *traceroute, int probe, int received_bytes[MAX_PROBES], struct sockaddr_in *received_addr[MAX_PROBES])
{
	static struct in_addr *last_addr	= NULL;
	char *ip							= NULL;
	
	if (received_addr[probe])
		ip = inet_ntoa(received_addr[probe]->sin_addr);

	// DNS resolution
	char *host					= NULL;
	struct hostent *recv_host	= NULL;
	if (received_addr[probe])
		recv_host = gethostbyaddr(&received_addr[probe]->sin_addr, sizeof(received_addr[probe]->sin_addr), AF_INET);
	if (recv_host)
		host = recv_host->h_name;
	else
		host = ip;

	// Show ip address
	if (!last_addr && received_bytes[probe] >= 0)
	{
		printf("  %s", ip);
		if (traceroute->flag->resolve_hostname)
		{
			if (host)
				printf(" (%s)", host);
			else
				printf(" (%s)", ip);
		}
		last_addr = &received_addr[probe]->sin_addr;
	}
	else if (last_addr && received_addr[probe] && last_addr->s_addr != received_addr[probe]->sin_addr.s_addr)
	{
		printf("  %s", ip);
		if (traceroute->flag->resolve_hostname)
		{
			if (host)
				printf(" (%s)", host);
			else
				printf(" (%s)", ip);
		}
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

	if (probe == traceroute->flag->q - 1)
	{
		printf("\n");
		last_addr = NULL;
	}
	fflush(stdout);
}

void	print_usage()
{
	printf("Usage: ./ft_traceroute [-V] [-m NUM] [-p PORT] [-q NUM] [-t NUM]\n");
	printf("                 [--resolve-hostnames] [--help] [--usage]\n");
}

void	print_help()
{
	printf("Usage: ./ft_traceroute [OPTION...] HOST\n");
	printf("Print the route packets trace to network host.\n");
	printf("\n");

	PRINT_OPT_S("-m,", "set maximal hop count (default: 64)");
	PRINT_OPT_S("-p,", "use destination PORT port (default: 33434)");
	PRINT_OPT_L("--resolve-hostnames", "resolve hostnames");
	PRINT_OPT_S("-q,", "send NUM probe packets per hop (default: 3)");
	PRINT_OPT_S("-t,", "set type of service (TOS) to NUM");
	PRINT_OPT_L("--help,", "give this help list");
	PRINT_OPT_L("--usage,", "give a short usage message");
	PRINT_OPT_S("-V,", "print program version");
}
