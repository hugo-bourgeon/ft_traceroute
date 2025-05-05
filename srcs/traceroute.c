/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traceroute.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 19:46:32 by hubourge          #+#    #+#             */
/*   Updated: 2025/05/05 14:38:34 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

void	ft_traceroute(t_traceroute *traceroute)
{
	signal(SIGINT, handle_sigint);
	print_header(traceroute);
	
	for (int ttl = 1; ttl <= traceroute->flag->m; ttl++)
	{
		check_sigint(traceroute);
		
		struct sockaddr_in	*received_addr[traceroute->flag->m];
		int 				received_bytes[traceroute->flag->m];
		ft_memset(received_addr, 0, sizeof(received_addr));
		ft_memset(received_bytes, 0, sizeof(received_bytes));

		printf(" %2d ", ttl);
		fflush(stdout);
		for (int probe = 0; probe < traceroute->flag->q; probe++)
		{
			char packet[ICMP_PACKET_SIZE];
			
			init_ttl(traceroute, ttl);
			init_packet_dest(packet, ttl, traceroute);
			
			check_sigint(traceroute);
			handle_send(traceroute, packet);
			
			check_sigint(traceroute);
			handle_receive(traceroute, probe, received_bytes, received_addr);

			print_stats(traceroute, probe, received_bytes, received_addr);
		}
		
		if (traceroute->flag->type == TYPE_UDP \
			&& received_addr[0] != NULL \
			&& memcmp(&((struct sockaddr_in *)traceroute->udp_dest_result->ai_addr)->sin_addr,
				   &received_addr[0]->sin_addr,
				   sizeof(struct in_addr)) == 0)
			break;
		else if (traceroute->flag->type == TYPE_ICMP \
			&& received_addr[0] != NULL \
			&& memcmp(&((struct sockaddr_in *)traceroute->icmp_dest_result->ai_addr)->sin_addr,
				   &received_addr[0]->sin_addr,
				   sizeof(struct in_addr)) == 0)
			break;
	}
}

void	handle_send(t_traceroute *traceroute, char packet[ICMP_PACKET_SIZE])
{
	if (traceroute->flag->type == TYPE_UDP)
	{
		gettimeofday(&traceroute->start, NULL);
		if (sendto(traceroute->send_sockfd, packet, UDP_PACKET_SIZE, 0, traceroute->udp_dest_result->ai_addr, traceroute->udp_dest_result->ai_addrlen) < 0)
		{
			fprintf(stderr, "  sendto error: %s\n", strerror(errno));
			free_all(EXIT_FAILURE, traceroute);
		}
	}
	else
	{
		gettimeofday(&traceroute->start, NULL);
		if (sendto(traceroute->icmp_sockfd, packet, ICMP_PACKET_SIZE, 0, traceroute->icmp_dest_result->ai_addr, traceroute->icmp_dest_result->ai_addrlen) < 0)
		{
			fprintf(stderr, "  sendto error: %s\n", strerror(errno));
			free_all(EXIT_FAILURE, traceroute);
		}
	}
}

void	handle_receive(t_traceroute *traceroute, int probe, int received_bytes[MAX_PROBES], struct sockaddr_in *received_addr[MAX_PROBES])
{
	char        recvbuf[RECV_BUFFER_SIZE];
	socklen_t   sender_len  = sizeof(traceroute->recv_addr);

	fd_set readfds;
	struct timeval timeout;
	FD_ZERO(&readfds);
	if (traceroute->flag->type == TYPE_UDP)
		FD_SET(traceroute->recv_sockfd, &readfds);
	else
		FD_SET(traceroute->icmp_sockfd, &readfds);

	timeout.tv_sec	= traceroute->flag->w;
	timeout.tv_usec	= 0;
	if (traceroute->flag->type == TYPE_UDP)
	{
		int ready = select(traceroute->recv_sockfd + 1, &readfds, NULL, NULL, &timeout);
		if (ready > 0)
		{
			received_bytes[probe] = recvfrom(traceroute->recv_sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&traceroute->recv_addr, &sender_len);
			gettimeofday(&traceroute->end, NULL);
			received_addr[probe] = &traceroute->recv_addr;
		}
		else // Timeout
		{
			received_bytes[probe] = -1;
			received_addr[probe] = NULL;
		}

	}
	else
	{
		int ready = select(traceroute->icmp_sockfd + 1, &readfds, NULL, NULL, &timeout);
		if (ready > 0)
		{
			received_bytes[probe] = recvfrom(traceroute->icmp_sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&traceroute->recv_addr, &sender_len);
			gettimeofday(&traceroute->end, NULL);
			received_addr[probe] = &traceroute->recv_addr;
		}
		else // Timeout
		{
			received_bytes[probe] = -1;
			received_addr[probe] = NULL;
		}
	}
}