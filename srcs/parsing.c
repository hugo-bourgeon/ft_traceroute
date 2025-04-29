/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:05:00 by hubourge          #+#    #+#             */
/*   Updated: 2025/04/29 11:41:06 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

void	parse_m(char *optarg, t_traceroute *traceroute);
void	parse_p(char *optarg, t_traceroute *traceroute);
void	parse_q(char *optarg, t_traceroute *traceroute);
void	parse_t(char *optarg, t_traceroute *traceroute);
void	parse_V(t_traceroute *traceroute);
void	parse_help(t_traceroute *traceroute);
void	parse_usage(t_traceroute *traceroute);

void	parsing(int argc, char **argv, t_traceroute *traceroute)
{
	if (argc < 2)
	{
		fprintf(stderr, "./ft_traceroute: missing host operand\n");
		fprintf(stderr, "Try './ft_traceroute --help' or './ft_traceroute --usage' for more information.\n");
		free_all(EXIT_FAILURE, traceroute);
	}

	int						opt;
	static struct option	long_options[] = 
	{
		{"usage",				no_argument,	0,	FLAG_USAGE},
		{"help",				no_argument,	0,	FLAG_HELP},
		{"resolve-hostnames",	no_argument,	0,	FLAG_RESOLVE_HOSTNAME},
		{0,						0,				0,	0}
	};

	while ((opt = getopt_long(argc, argv, "V :m:p:q:t:", long_options, NULL)) != -1)
	{
		switch (opt)
		{
			case 'm': parse_m(optarg, traceroute); break;
			case 'p': parse_p(optarg, traceroute); break;
			case 'q': parse_q(optarg, traceroute); break;
			case 't': parse_t(optarg, traceroute); break;
			case 'V': parse_V(traceroute); break;

			case FLAG_USAGE:			parse_usage(traceroute); break;
			case FLAG_HELP:				parse_help(traceroute); break;
			case FLAG_RESOLVE_HOSTNAME:	traceroute->flag->resolve_hostname = 1; break;
		}
	}

	if (optind >= argc)
	{
		fprintf(stderr, "Error: missing host operand\n");
		free_all(EXIT_FAILURE, traceroute);
	}
	
	printf("optind: %d\n", optind);
	printf("traceroute host: %s\n", argv[optind]);
	
	traceroute->hostname = strdup(argv[optind]);
	if (!traceroute->hostname)
	{
		fprintf(stderr, "strdup error\n");
		free_all(EXIT_FAILURE, traceroute);
	}
}


void	parse_m(char *optarg, t_traceroute *traceroute)
{
	(void)optarg;
	(void)traceroute;
}

void	parse_p(char *optarg, t_traceroute *traceroute)
{
	(void)optarg;
	(void)traceroute;
}

void	parse_q(char *optarg, t_traceroute *traceroute)
{
	(void)optarg;
	(void)traceroute;
}

void	parse_t(char *optarg, t_traceroute *traceroute)
{
	(void)optarg;
	(void)traceroute;
}

void	parse_V(t_traceroute *traceroute)
{
	(void)optarg;
	(void)traceroute;
}

void	parse_help(t_traceroute *traceroute)
{
	print_help();
	free_all(EXIT_SUCCESS, traceroute);
}

void	parse_usage(t_traceroute *traceroute)
{
	print_usage();
	free_all(EXIT_SUCCESS, traceroute);
}
