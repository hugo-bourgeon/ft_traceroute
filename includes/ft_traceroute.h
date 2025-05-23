/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_traceroute.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 15:37:35 by hubourge          #+#    #+#             */
/*   Updated: 2025/05/13 18:46:53 by hubourge         ###   ########.fr       */
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
# include <netinet/udp.h>
# include <netdb.h>
# include <signal.h>
# include <getopt.h>
# include <ctype.h>
# include <errno.h>

# define NOT_EXIT				-1
# define PROCESS				0
# define STOP					1
# define DEFAULT_SET			-1

# define ICMP_HDRLEN			8
# define ICMP_PAYLOAD_LEN		24
# define ICMP_PACKET_SIZE		(ICMP_HDRLEN + ICMP_PAYLOAD_LEN)
# define UDP_PAYLOAD_LEN		9
# define UDP_PACKET_SIZE		(UDP_PAYLOAD_LEN)
# define RECV_BUFFER_SIZE		512

# define DEFAULT_PROBES			3
# define MAX_PROBES				10
# define DEFAULT_HOPS			64
# define DEFAULT_TIMEOUT		3
# define DEFAULT_PORT			33434

# define FLAG_USAGE				1
# define FLAG_HELP				2
# define FLAG_RESOLVE_HOSTNAME	3
# define FLAG_TYPE				4

# define TYPE_ICMP				1
# define TYPE_UDP				2

# define PRINT_OPT(short, long, msg)	printf("  %-4s %-21s %s\n", short, long, msg);
# define PRINT_OPT_S(short, msg)		PRINT_OPT(short, "", msg)
# define PRINT_OPT_L(long, msg)			PRINT_OPT("", long, msg)

extern int	g_stop_code;

typedef struct s_flag
{
	int				m;
	int				p;
	int				q;
	int				t;
	int				w;
	int				resolve_hostname;
	int				type;
	int				V;
	int 			usage;
	int				help;
}	t_flag;

typedef struct s_traceroute
{
	char				*hostname;
	char				*ip;
	int					icmp_sockfd;
	int					send_sockfd;
	int					recv_sockfd;
	struct sockaddr_in	recv_addr;
	struct icmp			*dest_icmp_hdr;
	struct addrinfo		icmp_dest_hints;
	struct addrinfo		*icmp_dest_result;
	struct addrinfo		udp_dest_hints;
	struct addrinfo		*udp_dest_result;
	struct timeval		start;
	struct timeval		end;
	int					dest_host_unreachable;
	t_flag				*flag; 
}	t_traceroute;

// init.c
void			init(t_traceroute **traceroute);
void			init_packet_dest(char packet[ICMP_PACKET_SIZE], int ttl, t_traceroute *traceroute);
void			init_dest(t_traceroute *traceroute);
void			init_socket(t_traceroute *traceroute);
void			init_ttl(t_traceroute *traceroute, int ttl);

// parsing.c
void			parsing(int argc, char **argv, t_traceroute *traceroute);
void			parse_m(char *optarg, t_traceroute *traceroute);
void			parse_M(char *optarg, t_traceroute *traceroute);
void			parse_p(char *optarg, t_traceroute *traceroute);
void			parse_q(char *optarg, t_traceroute *traceroute);
void			parse_t(char *optarg, t_traceroute *traceroute);
void			parse_w(char *optarg, t_traceroute *traceroute);
void			parse_V(t_traceroute *traceroute);
void			parse_help(t_traceroute *traceroute);
void			parse_usage(t_traceroute *traceroute);

// traceroute.c
void			ft_traceroute(t_traceroute *traceroute);
void			handle_send(t_traceroute *traceroute, char packet[ICMP_PACKET_SIZE]);
void			handle_receive(t_traceroute *traceroute, int probe, int received_bytes[MAX_PROBES], struct sockaddr_in *received_addr[MAX_PROBES]);

// print.c
void			print_header(t_traceroute *traceroute);
void			print_stats(t_traceroute *traceroute, int probe, int received_bytes[MAX_PROBES], struct sockaddr_in *received_addr[MAX_PROBES]);
void			print_usage();
void			print_help();

// utils.c
void			free_all(int exit_code, t_traceroute *traceroute);
unsigned short	checksum(void *packet, int len);
void			handle_sigint(int sig);
void			check_sigint(t_traceroute *traceroute);
void			check_dest_host_unreachable(t_traceroute *traceroute, int len, const char *recvbuf);

#endif
