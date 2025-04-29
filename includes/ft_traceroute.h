/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_traceroute.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 15:37:35 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/29 11:10:42 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_TRACEROUTE_H$
# define FT_TRACEROUTE_H$

# include "../libft/libft.h"
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/ip_icmp.h>
# include <netdb.h>
# include <signal.h>
# include <getopt.h>

# define NOT_EXIT				-1
# define PROCESS				0
# define STOP					1

# define PACKET_SIZE			45
# define RECV_BUFFER_SIZE		512

# define DEFAULT_PROBES			3
# define DEFAULT_HOPS			64
# define DEFAULT_TIMEOUT		3

# define FLAG_USAGE				1
# define FLAG_HELP				2
# define FLAG_RESOLVE_HOSTNAME	3

extern int	g_stop_code;

typedef struct s_flag
{
	int				m;
	int				p;
	int				q;
	int				t;
	int				resolve_hostname;
	int				V;
	int 			usage;
	int				help;
}	t_flag;

typedef struct s_traceroute
{
	char				*hostname;
	char				*ip;
	int					sockfd;
	struct sockaddr_in	recv_addr; 
	struct icmp			*dest_icmp_hdr;
	struct addrinfo		dest_hints;
	struct addrinfo		*dest_result;
	struct timeval		start;
	struct timeval		end;
	t_flag				*flag; 
}	t_traceroute;

// traceroute.c
void			ft_traceroute(t_traceroute *traceroute);
void			handle_send(t_traceroute *traceroute, char packet[PACKET_SIZE]);
void			handle_receive(t_traceroute *traceroute, int probe, int received_bytes[DEFAULT_PROBES], struct sockaddr_in *received_addr[DEFAULT_PROBES]);

// init.c
void			init(t_traceroute **traceroute);
void			init_packet_icmp_header(char packet[PACKET_SIZE], int ttl, t_traceroute *traceroute);
void			init_dest(t_traceroute *traceroute);
void			init_socket(t_traceroute *traceroute);
void			init_ttl(t_traceroute *traceroute, int ttl);

// parsing.c
void			parsing(int argc, char **argv, t_traceroute *traceroute);

// utils.c
void			free_all(int exit_code, t_traceroute *traceroute);
unsigned short	checksum(void *packet, int len);
void			handle_sigint(int sig);
void			check_sigint(t_traceroute *traceroute);

// print.c
void			print_header(t_traceroute *traceroute);
void			print_stats(t_traceroute *traceroute, int probe, int received_bytes[DEFAULT_PROBES], struct sockaddr_in *received_addr[DEFAULT_PROBES]);

#endif
