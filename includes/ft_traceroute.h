/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_traceroute.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 15:37:35 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/25 19:12:31 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_TRACEROUTE_H$
# define FT_TRACEROUTE_H$

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>

#define MAX_HOPS			30
#define TIMEOUT_SEC			1
#define PACKET_SIZE			64
#define RECV_BUFFER_SIZE	512

# define NOT_EXIT			-1

typedef struct s_traceroute
{
	char				*hostname;
	struct sockaddr_in	recv_addr; 
	struct sockaddr_in	dest;
	struct icmp			*dest_icmp_hdr;
	struct addrinfo		dest_hints;
	struct addrinfo		*dest_result;
	struct timeval		start;
	struct timeval		end;
}	t_traceroute;

void			init(t_traceroute **traceroute);

void			parsing(int argc, char **argv, t_traceroute *traceroute);

void			free_all(int exit_code, t_traceroute *traceroute);
unsigned short	checksum(void *packet, int len);


#endif