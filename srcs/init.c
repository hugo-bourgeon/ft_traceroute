/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:04:53 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/27 18:48:56 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

void	init(t_traceroute **traceroute)
{
	*traceroute = malloc(sizeof(t_traceroute));
	if (!(*traceroute))
	{
		perror("malloc");
		free_all(EXIT_FAILURE, *traceroute);
	}
	(*traceroute)->hostname	= NULL;
	(*traceroute)->ip		= NULL;
}

void	init_icmp_header(char packet[PACKET_SIZE], int ttl, t_traceroute *traceroute)
{
	traceroute->dest_icmp_hdr = (struct icmp *) packet;
	traceroute->dest_icmp_hdr->icmp_type     = ICMP_ECHO;
	traceroute->dest_icmp_hdr->icmp_code     = 0;
	traceroute->dest_icmp_hdr->icmp_id       = getpid() & 0xFFFF;
	traceroute->dest_icmp_hdr->icmp_seq      = ttl;
	traceroute->dest_icmp_hdr->icmp_cksum    = checksum(packet, PACKET_SIZE);
}
