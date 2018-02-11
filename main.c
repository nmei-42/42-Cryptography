/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/29 22:12:35 by nmei              #+#    #+#             */
/*   Updated: 2018/02/10 20:34:48 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <libft.h>
#include "ft_ssl.h"
#define NUM_CMDS 7

static const t_cmd_lookup g_cmd_lookup[NUM_CMDS] =
{
	{"base64", &handle_base64, CIPHER},
	{"des", &handle_des, CIPHER},
	{"des-ecb", &handle_des, CIPHER},
	{"des-cbc", &handle_des, CIPHER},
	{"des3", &handle_des, CIPHER},
	{"des3-ecb", &handle_des, CIPHER},
	{"des3-cbc", &handle_des, CIPHER}
};

static void	disp_cmds(t_cmd_type cmd_type)
{
	int i;
	int found;

	i = 0;
	found = 0;
	while (i < NUM_CMDS)
	{
		if (g_cmd_lookup[i].cmd_type == cmd_type)
		{
			found = 1;
			ft_dprintf(2, "%s\n", g_cmd_lookup[i].name);
		}
		i++;
	}
	if (found == 0)
		ft_dprintf(2, "\n");
}

static void	ft_ssl_usage(int mode, char *argv)
{
	if (mode == 0)
		ft_dprintf(2, "Usage: ft_ssl command [command opts] [command args]\n");
	else if (mode == 1 && argv)
	{
		ft_dprintf(2, "ft_ssl: '%s' is an invalid command.\n", argv);
		ft_dprintf(2, "Standard commands:\n");
		disp_cmds(STANDARD);
		ft_dprintf(2, "Message Digest commands:\n");
		disp_cmds(MESSAGE_DIGEST);
		ft_dprintf(2, "Cipher commands:\n");
		disp_cmds(CIPHER);
		ft_dprintf(2, "\nFor more detailed usage use the -h option"
						" with specific commands!\n");
	}
	exit(1);
}

/*
**	main()
*/

int			main(int argc, char **argvs)
{
	int i;
	int	found;

	if (argc > 1 && *argvs[1])
	{
		i = -1;
		found = 0;
		while (++i < NUM_CMDS)
		{
			if (strcmp(g_cmd_lookup[i].name, argvs[1]) == 0)
			{
				found = 1;
				g_cmd_lookup[i].cmd_func(argvs[1], argc, argvs);
			}
		}
		if (found == 0)
			ft_ssl_usage(1, argvs[1]);
	}
	else
		ft_ssl_usage(0, NULL);
}
