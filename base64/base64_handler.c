/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base64_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/30 20:06:38 by nmei              #+#    #+#             */
/*   Updated: 2018/02/11 08:45:46 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <libft.h>
#include <base64.h>

static int	get_compound_arg(t_b64_opts *opt, char **path, int *i, char *o_type)
{
	if (ft_strcmp(opt->argvs[*i], o_type) == 0)
	{
		opt->flags |= (ft_strcmp("-i", o_type) == 0) ? I_FLAG : 0;
		opt->flags |= (ft_strcmp("-o", o_type) == 0) ? O_FLAG : 0;
		if (*i + 1 < opt->argc)
			*path = opt->argvs[*i + 1];
		else
		{
			ft_dprintf(2, "%s: Option [%s] requires an argument.\n",
						opt->cmd_name, o_type);
			exit(1);
		}
		*i += 2;
		return (1);
	}
	return (0);
}

void		disp_b64_usage(void)
{
	ft_dprintf(2, "\nUsage: ./ft_ssl base64 [b64 opts] [b64 args]\n");
	ft_dprintf(2, "[-e]: Encrypt input to base64\n");
	ft_dprintf(2, "[-d]: Decrypt base64 input to plaintext\n");
	ft_dprintf(2, "[-i]: Specify input stream\n");
	ft_dprintf(2, "[-o]: Specify output stream\n");
	ft_dprintf(2, "[-h]: Displays base64 usage (like right now!)\n\n");
	ft_dprintf(2, "Options not in this list will be **ignored**!!\n\n");
	exit(0);
}

void		parse_b64_args(t_b64_opts *opt)
{
	int		i;

	i = 2;
	while (i < opt->argc)
	{
		opt->flags |= (ft_strcmp(opt->argvs[i], "-e") == 0) ? E_FLAG : 0;
		opt->flags |= (ft_strcmp(opt->argvs[i], "-d") == 0) ? D_FLAG : 0;
		opt->flags |= (ft_strcmp(opt->argvs[i], "-h") == 0) ? H_FLAG : 0;
		if (opt->flags & H_FLAG)
			disp_b64_usage();
		if (get_compound_arg(opt, &opt->ipath, &i, "-i"))
			continue ;
		if (get_compound_arg(opt, &opt->opath, &i, "-o"))
			continue ;
		i++;
	}
}

void		check_b64_args(t_b64_opts *opt)
{
	if ((opt->flags & E_FLAG) && (opt->flags & D_FLAG))
	{
		ft_dprintf(2, "%s: Can't have both [-e] and [-d]!\n", opt->cmd_name);
		exit(1);
	}
	if (!((opt->flags & E_FLAG) || (opt->flags & D_FLAG)))
	{
		ft_dprintf(2, "%s: No [-e] or [-d] option detected. "
						"Defaulting to [-e].\n", opt->cmd_name);
		opt->flags |= E_FLAG;
	}
	if ((opt->flags & I_FLAG) && (opt->in_fd = open(opt->ipath, O_RDONLY)) < 0)
	{
		ft_dprintf(2, "%s: Unable to open [%s] for reading.\n",
						opt->cmd_name, opt->ipath);
		exit(1);
	}
	if ((opt->flags & O_FLAG) && (opt->out_fd = open(opt->opath,
	O_TRUNC | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
	{
		ft_dprintf(2, "%s: Unable to create or open [%s] for writing.\n",
						opt->cmd_name, opt->opath);
		exit(1);
	}
	opt->out_fd = (opt->out_fd == 0) ? 1 : opt->out_fd;
}

/*
**	handle_base64()
**	Handler function that calls the correct functions depending on if we're
**	encrypting or decrypting.
**
**	NOTE: we use the 'write()' function for base64 decryption becuase if someone
**	evil decides to encrypt a string like: "Test\0x00\0x00\0x00I'm Hidden" then
**	we want to write out all bytes even the 'hidden ones'. My ft_printf
**	unfortunately faceplants on this task...
*/

void		handle_base64(char *name, int argc, char **argvs)
{
	t_b64_opts	b64_opt;
	ssize_t		in_len;
	char		*input;
	ssize_t		out_len;
	char		*output;

	ft_bzero(&b64_opt, sizeof(b64_opt));
	b64_opt.cmd_name = name;
	b64_opt.argc = argc;
	b64_opt.argvs = argvs;
	parse_b64_args(&b64_opt);
	check_b64_args(&b64_opt);
	in_len = ft_read_all(b64_opt.in_fd, &input);
	if (b64_opt.flags & E_FLAG)
	{
		output = encode_base64(input, in_len, &out_len);
		base64_print_encrypted(b64_opt.out_fd, output, out_len);
	}
	else
	{
		output = decode_base64(input, in_len, &out_len);
		write(b64_opt.out_fd, output, out_len);
	}
	free(output);
	close(b64_opt.out_fd);
}
