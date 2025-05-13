/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:04:53 by hubourge          #+#    #+#             */
/*   Updated: 2025/05/13 17:46:28 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

void	init(t_traceroute **traceroute)
{
	*traceroute = malloc(sizeof(t_traceroute));
	if (!(*traceroute))
	{
		fprintf(stderr, "sendto error: %s\n", strerror(errno));
		free_all(EXIT_FAILURE, *traceroute);
	}
	(*traceroute)->hostname	= NULL;
	(*traceroute)->ip		= NULL;
	(*traceroute)->dest_icmp_hdr = NULL;
	(*traceroute)->icmp_dest_result = NULL;
	(*traceroute)->udp_dest_result = NULL;
	(*traceroute)->icmp_sockfd = 0;
	(*traceroute)->send_sockfd = 0;
	(*traceroute)->recv_sockfd = 0;
	(*traceroute)->dest_host_unreachable = 0;

	(*traceroute)->flag = malloc(sizeof(t_flag));
	if	(!(*traceroute)->flag)
	{
		fprintf(stderr, "sendto error: %s\n", strerror(errno));
		free_all(EXIT_FAILURE, *traceroute);
	}
	(*traceroute)->flag->m = DEFAULT_HOPS;
	(*traceroute)->flag->p = DEFAULT_PORT;
	(*traceroute)->flag->q = DEFAULT_PROBES;
	(*traceroute)->flag->t = DEFAULT_SET;
	(*traceroute)->flag->w = DEFAULT_TIMEOUT;
	(*traceroute)->flag->resolve_hostname = DEFAULT_SET;
	(*traceroute)->flag->V = DEFAULT_SET;
	(*traceroute)->flag->usage = DEFAULT_SET;
	(*traceroute)->flag->help = DEFAULT_SET;
	(*traceroute)->flag->type = TYPE_UDP;
}

void	init_packet_dest(char packet[ICMP_PACKET_SIZE], int ttl, t_traceroute *traceroute)
{
	ft_memset(packet, 0, ICMP_PACKET_SIZE);
	if (traceroute->flag->type == TYPE_UDP)
	{
		((struct sockaddr_in *)traceroute->udp_dest_result->ai_addr)->sin_port = htons(traceroute->flag->p + ttl - 1);

		for (int i = 0; i < UDP_PACKET_SIZE; i++)
			packet[i] = (uint8_t)i;
	}
	else if (traceroute->flag->type == TYPE_ICMP)
	{
		traceroute->dest_icmp_hdr = (struct icmp *) packet;
		traceroute->dest_icmp_hdr->icmp_type     	= ICMP_ECHO;
		traceroute->dest_icmp_hdr->icmp_code     	= 0;
		traceroute->dest_icmp_hdr->icmp_id       	= getpid() & 0xFFFF;
		traceroute->dest_icmp_hdr->icmp_seq      	= htons(ttl - 1);
		
		// Fill payload with incremental values
		for (int i = 0; i < ICMP_PAYLOAD_LEN; i++)
			packet[ICMP_HDRLEN + i] = (uint8_t)(i + 8);
		
		traceroute->dest_icmp_hdr->icmp_cksum		= 0;
		traceroute->dest_icmp_hdr->icmp_cksum    	= checksum(packet, ICMP_PACKET_SIZE);
	}
}

void	init_dest(t_traceroute *traceroute)
{
	// Initialize the destination address
	if (traceroute->flag->type == TYPE_UDP)
	{
		// Initialize the destination
		ft_memset(&traceroute->udp_dest_hints, 0, sizeof(traceroute->udp_dest_hints));
		traceroute->udp_dest_hints.ai_family = AF_INET;
		traceroute->udp_dest_hints.ai_socktype = SOCK_DGRAM;
		traceroute->udp_dest_hints.ai_protocol = IPPROTO_UDP;

		// Get the destination address
		int ret = getaddrinfo(traceroute->hostname, NULL, &traceroute->udp_dest_hints, &traceroute->udp_dest_result);
		if (ret != 0)
		{
			fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
			free_all(EXIT_FAILURE, traceroute);
		}
		
		// Get the IP address
		struct sockaddr_in *addr = (struct sockaddr_in *)traceroute->udp_dest_result->ai_addr;
		traceroute->ip = strdup(inet_ntoa(addr->sin_addr));
		if (!traceroute->ip)
		{
			fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
			free_all(EXIT_FAILURE, traceroute);
		}
	}
	else if (traceroute->flag->type == TYPE_ICMP)
	{
		// Initialize the destination
		ft_memset(&traceroute->icmp_dest_hints, 0, sizeof(traceroute->icmp_dest_hints));
		traceroute->icmp_dest_hints.ai_family = AF_INET;
		traceroute->icmp_dest_hints.ai_socktype = SOCK_RAW;
		traceroute->icmp_dest_hints.ai_protocol = IPPROTO_ICMP;
		
		// Get the destination address
		int ret = getaddrinfo(traceroute->hostname, NULL, &traceroute->icmp_dest_hints, &traceroute->icmp_dest_result);
		if (ret != 0)
		{
			fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
			free_all(EXIT_FAILURE, traceroute);
		}
	
		// Get the IP address
		struct sockaddr_in *addr = (struct sockaddr_in *)traceroute->icmp_dest_result->ai_addr;
		traceroute->ip = strdup(inet_ntoa(addr->sin_addr));
		if (!traceroute->ip)
		{
			fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
			free_all(EXIT_FAILURE, traceroute);
		}
	}
}

void	init_socket(t_traceroute *traceroute)
{
	// Create a raw socket
	if (traceroute->flag->type == TYPE_UDP)
	{
		traceroute->send_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		traceroute->recv_sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
		if (traceroute->send_sockfd < 0 || traceroute->recv_sockfd < 0)
		{
			fprintf(stderr, "sendto error: %s\n", strerror(errno));
			free_all(EXIT_FAILURE, traceroute);
		}
	}
	else if (traceroute->flag->type == TYPE_ICMP)
	{
		traceroute->icmp_sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
		if (traceroute->icmp_sockfd < 0)
		{
			fprintf(stderr, "sendto error: %s\n", strerror(errno));
			free_all(EXIT_FAILURE, traceroute);
		}
	}

	// Set the TOS (Type of Service) if specified
	if (traceroute->flag->t != DEFAULT_SET)
	{
		int tos = traceroute->flag->t;
		if (traceroute->flag->type == TYPE_ICMP \
			&& setsockopt(traceroute->icmp_sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) < 0)
		{
			fprintf(stderr, "sendto error: %s\n", strerror(errno));
			free_all(EXIT_FAILURE, traceroute);
		}
		else if (traceroute->flag->type == TYPE_UDP \
			&& setsockopt(traceroute->send_sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) < 0)
		{
			fprintf(stderr, "sendto error: %s\n", strerror(errno));
			free_all(EXIT_FAILURE, traceroute);
		}
	}
}

void	init_ttl(t_traceroute *traceroute, int ttl)
{
	// Set the TTL (Time to Live) for the packet
	if (traceroute->flag->type == TYPE_UDP \
		&& setsockopt(traceroute->send_sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
	{
		fprintf(stderr, "sendto error: %s\n", strerror(errno));
		free_all(EXIT_FAILURE, traceroute);
	}
	else if (traceroute->flag->type == TYPE_ICMP \
		&& setsockopt(traceroute->icmp_sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
	{
		fprintf(stderr, "sendto error: %s\n", strerror(errno));
		free_all(EXIT_FAILURE, traceroute);
	}
}