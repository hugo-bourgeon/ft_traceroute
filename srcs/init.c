/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:04:53 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/29 11:35:48 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

void	init(t_traceroute **traceroute)
{
	*traceroute = malloc(sizeof(t_traceroute));
	if (!(*traceroute))
	{
		fprintf(stderr, "malloc error\n");
		free_all(EXIT_FAILURE, *traceroute);
	}
	(*traceroute)->hostname	= NULL;
	(*traceroute)->ip		= NULL;
	(*traceroute)->sockfd	= 0;
	(*traceroute)->dest_icmp_hdr = NULL;
	(*traceroute)->dest_result = NULL;
	
	(*traceroute)->flag = malloc(sizeof(t_flag));
	if	(!(*traceroute)->flag)
	{
		fprintf(stderr, "malloc error\n");
		free_all(EXIT_FAILURE, *traceroute);
	}
	(*traceroute)->flag->m = 0;
	(*traceroute)->flag->p = 0;
	(*traceroute)->flag->q = 0;
	(*traceroute)->flag->t = 0;
	(*traceroute)->flag->resolve_hostname = 0;
	(*traceroute)->flag->V = 0;
	(*traceroute)->flag->usage = 0;
	(*traceroute)->flag->help = 0;
}

void	init_packet_icmp_header(char packet[PACKET_SIZE], int ttl, t_traceroute *traceroute)
{
	ft_memset(packet, 0, PACKET_SIZE);
	traceroute->dest_icmp_hdr = (struct icmp *) packet;
	traceroute->dest_icmp_hdr->icmp_type     = ICMP_ECHO;
	traceroute->dest_icmp_hdr->icmp_code     = 0;
	traceroute->dest_icmp_hdr->icmp_id       = getpid() & 0xFFFF;
	traceroute->dest_icmp_hdr->icmp_seq      = ttl;
	traceroute->dest_icmp_hdr->icmp_cksum    = checksum(packet, PACKET_SIZE);
}

void	init_dest(t_traceroute *traceroute)
{
	// Initialize the destination address
	ft_memset(&traceroute->dest_hints, 0, sizeof(traceroute->dest_hints));
	traceroute->dest_hints.ai_family = AF_INET;
	traceroute->dest_hints.ai_socktype = SOCK_RAW;
	traceroute->dest_hints.ai_protocol = IPPROTO_ICMP;

	// Get the destination address
	int ret = getaddrinfo(traceroute->hostname, NULL, &traceroute->dest_hints, &traceroute->dest_result);
	if (ret != 0)
	{
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
		freeaddrinfo(traceroute->dest_result);
		free_all(EXIT_FAILURE, traceroute);
	}

	struct sockaddr_in *addr = (struct sockaddr_in *)traceroute->dest_result->ai_addr;
	traceroute->ip = strdup(inet_ntoa(addr->sin_addr));
	if (!traceroute->ip)
	{
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
		freeaddrinfo(traceroute->dest_result);
		free_all(EXIT_FAILURE, traceroute);
	}
}

void	init_socket(t_traceroute *traceroute)
{
	// Create a raw socket
	traceroute->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (traceroute->sockfd < 0)
	{
		fprintf(stderr, "socket error\n");
		freeaddrinfo(traceroute->dest_result);
		free_all(EXIT_FAILURE, traceroute);
	}

	// Set the socket timeout
	struct timeval timeout;
	timeout.tv_sec = DEFAULT_TIMEOUT;
	timeout.tv_usec = 0;
	if (setsockopt(traceroute->sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
	{
		fprintf(stderr, "setsockopt error\n");
		freeaddrinfo(traceroute->dest_result);
		free_all(EXIT_FAILURE, traceroute);
	}
}

void	init_ttl(t_traceroute *traceroute, int ttl)
{
	// Set the TTL (Time to Live) for the packet
	if (setsockopt(traceroute->sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
	{
		fprintf(stderr, "setsockopt error\n");
		freeaddrinfo(traceroute->dest_result);
		free_all(EXIT_FAILURE, traceroute);
	}
}