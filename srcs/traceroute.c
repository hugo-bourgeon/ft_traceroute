/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traceroute.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 19:46:32 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/29 10:27:54 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

void	ft_traceroute(t_traceroute *traceroute)
{
	signal(SIGINT, handle_sigint);
	print_header(traceroute);
	
	for (int ttl = 1; ttl <= DEFAULT_HOPS; ttl++)
	{
		check_sigint(traceroute);
		
		struct sockaddr_in	*received_addr[DEFAULT_HOPS];
		int 				received_bytes[DEFAULT_HOPS];
		ft_memset(received_addr, 0, sizeof(received_addr));
		ft_memset(received_bytes, 0, sizeof(received_bytes));

		printf(" %2d ", ttl);
		fflush(stdout);
		for (int probe = 0; probe < DEFAULT_PROBES; probe++)
		{
			char packet[PACKET_SIZE];
			
			init_ttl(traceroute, ttl);
			init_packet_icmp_header(packet, ttl, traceroute);

			check_sigint(traceroute);
			handle_send(traceroute, packet);
			
			check_sigint(traceroute);
			handle_receive(traceroute, probe, received_bytes, received_addr);

			print_stats(traceroute, probe, received_bytes, received_addr);
		}

		if (memcmp(&((struct sockaddr_in *)traceroute->dest_result->ai_addr)->sin_addr, &traceroute->recv_addr.sin_addr, sizeof(struct in_addr)) == 0)
			break;
	}
}

void	handle_send(t_traceroute *traceroute, char packet[PACKET_SIZE])
{
	gettimeofday(&traceroute->start, NULL);
	if (sendto(traceroute->sockfd, packet, PACKET_SIZE, 0, traceroute->dest_result->ai_addr, traceroute->dest_result->ai_addrlen) < 0)
	{
		fprintf(stderr, "sendto error\n");
		freeaddrinfo(traceroute->dest_result);
		free_all(EXIT_FAILURE, traceroute);
	}
}

void	handle_receive(t_traceroute *traceroute, int probe, int received_bytes[DEFAULT_PROBES], struct sockaddr_in *received_addr[DEFAULT_PROBES])
{
	char        recvbuf[RECV_BUFFER_SIZE];
	socklen_t   sender_len  = sizeof(traceroute->recv_addr);

	fd_set readfds;
	struct timeval timeout;
	FD_ZERO(&readfds);
	FD_SET(traceroute->sockfd, &readfds);

	timeout.tv_sec	= DEFAULT_TIMEOUT;
	timeout.tv_usec	= 0;

	int ready = select(traceroute->sockfd + 1, &readfds, NULL, NULL, &timeout);
	if (ready > 0)
	{
		received_bytes[probe] = recvfrom(traceroute->sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&traceroute->recv_addr, &sender_len);
		gettimeofday(&traceroute->end, NULL);
		received_addr[probe] = &traceroute->recv_addr;
	}
	else // Timeout
	{
		received_bytes[probe] = -1;
		received_addr[probe] = NULL;
	}
}