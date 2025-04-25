/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 15:36:01 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/25 19:17:16 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

int main(int argc, char **argv)
{
	t_traceroute	*traceroute;
	
	init(&traceroute);
	parsing(argc, argv, traceroute);
	
	// Initialize the destination address
	memset(&traceroute->dest_hints, 0, sizeof(traceroute->dest_hints));
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
	timeout.tv_sec = TIMEOUT_SEC;
	timeout.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	for (int ttl = 1; ttl <= MAX_HOPS; ttl++)
	{
		// Set the TTL (Time to Live) for the packet
		setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
		
		// Prepare the ICMP packet
		char packet[PACKET_SIZE];
		memset(packet, 0, PACKET_SIZE);

		// Initialize the ICMP header
		traceroute->dest_icmp_hdr = (struct icmp *) packet;
		traceroute->dest_icmp_hdr->icmp_type     = ICMP_ECHO;
		traceroute->dest_icmp_hdr->icmp_code     = 0;
		traceroute->dest_icmp_hdr->icmp_id       = getpid() & 0xFFFF;
		traceroute->dest_icmp_hdr->icmp_seq      = ttl;
		traceroute->dest_icmp_hdr->icmp_cksum    = checksum(packet, PACKET_SIZE);

		gettimeofday(&traceroute->start, NULL);
		if (sendto(sockfd, packet, PACKET_SIZE, 0, traceroute->dest_result->ai_addr, traceroute->dest_result->ai_addrlen) < 0)
		{
			fprintf(stderr, "sendto error\n");
			continue;
		}

		// Receive the response        
		char        recvbuf[RECV_BUFFER_SIZE];
		socklen_t   sender_len  = sizeof(traceroute->recv_addr);
		int         received    = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&traceroute->recv_addr, &sender_len);
		gettimeofday(&traceroute->end, NULL);

		// Get the IP address of the sender
		char *ip_str = inet_ntoa(traceroute->recv_addr.sin_addr);
		if (ip_str == NULL)
			fprintf(stderr, "inet_ntoa error\n");

		if (received < 0)
			printf("%2d  *\n", ttl);
		else
		{
			long elapsed =  (traceroute->end.tv_sec - traceroute->start.tv_sec) * 1000 +
							(traceroute->end.tv_usec - traceroute->start.tv_usec) / 1000;
			printf("%2d  %s  %ld ms\n", ttl, ip_str, elapsed);
			if (memcmp(&((struct sockaddr_in *)traceroute->dest_result->ai_addr)->sin_addr, &traceroute->recv_addr.sin_addr, sizeof(struct in_addr)) == 0)
				break;
		}
	}
	
	freeaddrinfo(traceroute->dest_result);
	free_all(NOT_EXIT, traceroute);

	return (0);
}
