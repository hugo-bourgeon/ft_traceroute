/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hubourge <hubourge@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 17:05:00 by hubourge          #+#    #+#             */
/*   Updated: 2025/05/13 18:46:30 by hubourge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_traceroute.h"

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

	while ((opt = getopt_long(argc, argv, "V :m:p:q:t:w:M:", long_options, NULL)) != -1)
	{
		switch (opt)
		{
			case 'm': parse_m(optarg, traceroute); break;
			case 'M': parse_M(optarg, traceroute); break;
			case 'p': parse_p(optarg, traceroute); break;
			case 'q': parse_q(optarg, traceroute); break;
			case 't': parse_t(optarg, traceroute); break;
			case 'w': parse_w(optarg, traceroute); break;
			case 'V': parse_V(traceroute); break;

			case FLAG_USAGE:			parse_usage(traceroute); break;
			case FLAG_HELP:				parse_help(traceroute); break;
			case FLAG_RESOLVE_HOSTNAME:	traceroute->flag->resolve_hostname = 1; break;

			case '?':
				fprintf(stderr, "Try './ft_traceroute --help' or './ft_traceroute --usage' for more information.\n");
				free_all(EXIT_FAILURE, traceroute);
				break;
		}
		
	}

	if (optind >= argc)
	{
		fprintf(stderr, "Error: missing host operand\n");
		free_all(EXIT_FAILURE, traceroute);
	}
	
	int idx = 0;
	for (int i = 0; i < argc; i++)
		idx++;

	traceroute->hostname = strdup(argv[idx - 1]);
	if (!traceroute->hostname)
	{
		fprintf(stderr, "strdup error\n");
		free_all(EXIT_FAILURE, traceroute);
	}
}

void	parse_m(char *optarg, t_traceroute *traceroute)
{
	if (atol(optarg) < 1 || atol(optarg) > 255 || strlen(optarg) > 3)
	{
		fprintf(stderr, "ft_traceroute: invalid hops value '%s'\n", optarg);
		free_all(EXIT_FAILURE, traceroute);
	}

	for (int i = 0; optarg[i]; i++)
	{
		if (!ft_isdigit(optarg[i]))
		{
			fprintf(stderr, "ft_traceroute: invalid hops value '%s'\n", optarg);
			free_all(EXIT_FAILURE, traceroute);
		}
	}

	traceroute->flag->m = atoi(optarg);
}

void	parse_M(char *optarg, t_traceroute *traceroute)
{
	if (ft_strncmp(optarg, "icmp", 5) == 0)
		traceroute->flag->type = TYPE_ICMP;
	else if (ft_strncmp(optarg, "udp", 4) == 0)
		traceroute->flag->type = TYPE_UDP;
	else
	{
		fprintf(stderr, "ft_traceroute: invalid method\n");
		fprintf(stderr, "Try './ft_traceroute --help' or './ft_traceroute --usage' for more information.\n");
		free_all(EXIT_FAILURE, traceroute);
	}
}

void	parse_p(char *optarg, t_traceroute *traceroute)
{
	if (atol(optarg) <= 0 || atol(optarg) >= 65537 || strlen(optarg) > 5)
	{
		fprintf(stderr, "ft_traceroute: invalid port number `%s'\n", optarg);
		free_all(EXIT_FAILURE, traceroute);
	}

	for (int i = 0; optarg[i]; i++)
	{
		if (!ft_isdigit(optarg[i]))
		{
			fprintf(stderr, "ft_traceroute: invalid port number `%s'\n", optarg);
			free_all(EXIT_FAILURE, traceroute);
		}
	}
	traceroute->flag->p = atoi(optarg);
}

void	parse_q(char *optarg, t_traceroute *traceroute)
{
	if (atol(optarg) < 1 || atol(optarg) > 10 || strlen(optarg) > 3)
	{
		fprintf(stderr, "ft_traceroute: number of tries should be between 1 and 10\n");
		free_all(EXIT_FAILURE, traceroute);
	}

	for (int i = 0; optarg[i]; i++)
	{
		if (!ft_isdigit(optarg[i]))
		{
			fprintf(stderr, "ft_traceroute: number of tries should be between 1 and 10\n");
			free_all(EXIT_FAILURE, traceroute);
		}
	}
	traceroute->flag->q = atoi(optarg);
}

void	parse_t(char *optarg, t_traceroute *traceroute)
{
	for (int i = 0; optarg[i]; i++)
	{
		if (!isdigit(optarg[i]) &&
			!(i == 1 && optarg[0] == '0' && (optarg[1] == 'x' || optarg[1] == 'X')) &&
			!(i > 1 && isxdigit(optarg[i]) && optarg[0] == '0' && (optarg[1] == 'x' || optarg[1] == 'X')))
		{
			fprintf(stderr, "ft_traceroute: invalid TOS value '%s'\n", optarg);
			free_all(EXIT_FAILURE, traceroute);
		}
	}
	char *endptr = NULL;
	long val = strtol(optarg, &endptr, 0);
	if (*endptr != '\0' || val < 0 || val > 255)
	{
		fprintf(stderr, "ft_traceroute: invalid TOS value '%s'\n", optarg);
		free_all(EXIT_FAILURE, traceroute);
	}
	
	traceroute->flag->t = (int)val;
}

void	parse_w(char *optarg, t_traceroute *traceroute)
{
	if (atol(optarg) < 0 || atol(optarg) > 60 || strlen(optarg) > 2)
	{
		fprintf(stderr, "ft_traceroute: ridiculous waiting time `%s'\n", optarg);
		free_all(EXIT_FAILURE, traceroute);
	}

	for (int i = 0; optarg[i]; i++)
	{
		if (!ft_isdigit(optarg[i]))
		{
			fprintf(stderr, "ft_traceroute: ridiculous waiting time `%s'\n", optarg);
			free_all(EXIT_FAILURE, traceroute);
		}
	}
	traceroute->flag->w = atoi(optarg);
}

void	parse_V(t_traceroute *traceroute)
{
	printf("ft_traceroute version 1.0 \n");
	printf("inspired by traceroute (GNU inetutils) 2.0\n");
	printf("Written by hubourge.\n");

	free_all(EXIT_SUCCESS, traceroute);
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
