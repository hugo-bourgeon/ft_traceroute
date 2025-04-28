/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 15:36:01 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/28 17:36:17 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

int main(int argc, char **argv)
{
	t_traceroute	*traceroute;
	
	init(&traceroute);
	parsing(argc, argv, traceroute);
	
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

	
	// Create a raw socket
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0)
	{
		fprintf(stderr, "socket error\n");
		freeaddrinfo(traceroute->dest_result);
		free_all(EXIT_FAILURE, traceroute);
	}

	// Set the socket timeout
	struct timeval timeout;
	timeout.tv_sec = DEFAULT_TIMEOUT;
	timeout.tv_usec = 0;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
	{
		fprintf(stderr, "setsockopt error\n");
		freeaddrinfo(traceroute->dest_result);
		free_all(EXIT_FAILURE, traceroute);
	}
	
	print_header(traceroute);
	for (int ttl = 1; ttl <= DEFAULT_HOPS; ttl++)
	{
		struct sockaddr_in	*received_addr[DEFAULT_HOPS];
		int 				received_bytes[DEFAULT_HOPS];
		ft_memset(received_addr, 0, sizeof(received_addr));
		ft_memset(received_bytes, 0, sizeof(received_bytes));
		
		printf("%2d", ttl);
		
		// Send all probes
		for (int probe = 0; probe < DEFAULT_PROBES; probe++)
		{
			// Set the TTL (Time to Live) for the packet
			if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
			{
				fprintf(stderr, "setsockopt error\n");
				freeaddrinfo(traceroute->dest_result);
				free_all(EXIT_FAILURE, traceroute);
			}

			// Prepare the ICMP packet
			char packet[PACKET_SIZE];
			ft_memset(packet, 0, PACKET_SIZE);

			// Initialize the ICMP header
			init_icmp_header(packet, ttl, traceroute);

			// Send the packet
			gettimeofday(&traceroute->start, NULL);
			if (sendto(sockfd, packet, PACKET_SIZE, 0, traceroute->dest_result->ai_addr, traceroute->dest_result->ai_addrlen) < 0)
			{
				fprintf(stderr, "sendto error\n");
				continue;
			}

			// Receive the response        
			char        recvbuf[RECV_BUFFER_SIZE];
			socklen_t   sender_len  = sizeof(traceroute->recv_addr);
			
			received_bytes[probe]			= recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&traceroute->recv_addr, &sender_len);
			gettimeofday(&traceroute->end, NULL);
			received_addr[probe] = &traceroute->recv_addr;
			print_stats(traceroute, probe, received_bytes, received_addr);
		}

		if (memcmp(&((struct sockaddr_in *)traceroute->dest_result->ai_addr)->sin_addr, &traceroute->recv_addr.sin_addr, sizeof(struct in_addr)) == 0)
			break;
	}
	
	freeaddrinfo(traceroute->dest_result);
	free_all(NOT_EXIT, traceroute);

	return (0);
}
