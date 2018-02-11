/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_handler.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/03 14:32:56 by nmei              #+#    #+#             */
/*   Updated: 2018/02/11 12:33:44 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <fcntl.h>
#include <libft.h>
#include <des.h>

static int	get_compnd_arg(t_des_opts *opt, char **opt_str, int *i,
							char *o_type)
{
	if (ft_strcmp(opt->argvs[*i], o_type) == 0)
	{
		opt->flags |= (ft_strcmp("-i", o_type) == 0) ? I_FLAG : 0;
		opt->flags |= (ft_strcmp("-o", o_type) == 0) ? O_FLAG : 0;
		opt->flags |= (ft_strcmp("-k", o_type) == 0) ? K_FLAG : 0;
		opt->flags |= (ft_strcmp("-v", o_type) == 0) ? V_FLAG : 0;
		if (*i + 1 < opt->argc)
		{
			if (*opt_str)
				free(*opt_str);
			*opt_str = ft_strdup(opt->argvs[*i + 1]);
		}
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

void		disp_des_usage(t_des_opts *opt, char *inval_opt)
{
	if (inval_opt)
		ft_dprintf(2, "%s: Unknown option [%s]\n", opt->cmd_name, inval_opt);
	ft_dprintf(2, "\nUsage: ./ft_ssl %s [%s opts] [%s args]\n",
					opt->cmd_name, opt->cmd_name, opt->cmd_name);
	ft_dprintf(2, "[-e]:\t\tEncrypt input with %s\n", opt->cmd_name);
	ft_dprintf(2, "[-d]:\t\tDecrypt %s input to plaintext\n", opt->cmd_name);
	ft_dprintf(2, "[-i]:\t\tSpecify input stream\n");
	ft_dprintf(2, "[-o]:\t\tSpecify output stream\n");
	ft_dprintf(2, "[-a]:\t\tBase64 process the data. If [-e] is specified"
	" then the output ciphertext is base64 encoded.\n\t\tIf [-d] is specified"
	" then input data is base64 decoded before decryption occurs.\n");
	ft_dprintf(2, "[-k]:\t\tThe actual key to use for encryption or decryption"
					" (Must be a string comprised of only hex digits).\n");
	ft_dprintf(2, "[-v]:\t\tSpecify an initialization vector to use with CBC"
	" encryption mode (Must be a string comprised of only hex digits).\n");
	ft_dprintf(2, "[-nopad]:\tDisable PKCS#7 padding of input data with [-e]."
	" Disables default padding removal from decrypted outputs with [-d].\n");
	ft_dprintf(2, "[-h]:\t\tDisplays %s usage (like now!)\n\n", opt->cmd_name);
	(opt->ipath) ? free(opt->ipath) : 0;
	(opt->opath) ? free(opt->opath) : 0;
	(opt->des_key) ? free(opt->des_key) : 0;
	(opt->des_ivec) ? free(opt->des_ivec) : 0;
	exit(0);
}

void		parse_des_args(t_des_opts *opt)
{
	int		i;
	int		f_start;

	i = 2;
	while (i < opt->argc)
	{
		f_start = opt->flags;
		opt->flags |= (ft_strcmp(opt->argvs[i], "-e") == 0) ? E_FLAG : 0;
		opt->flags |= (ft_strcmp(opt->argvs[i], "-d") == 0) ? D_FLAG : 0;
		opt->flags |= (ft_strcmp(opt->argvs[i], "-a") == 0) ? A_FLAG : 0;
		opt->flags |= (ft_strcmp(opt->argvs[i], "-nopad") == 0) ? NP_FLAG : 0;
		opt->flags |= (ft_strcmp(opt->argvs[i], "-h") == 0) ? H_FLAG : 0;
		if (get_compnd_arg(opt, &opt->ipath, &i, "-i"))
			continue ;
		if (get_compnd_arg(opt, &opt->opath, &i, "-o"))
			continue ;
		if (get_compnd_arg(opt, &opt->des_key, &i, "-k"))
			continue ;
		if ((opt->flags & CBC) && get_compnd_arg(opt, &opt->des_ivec, &i, "-v"))
			continue ;
		if ((opt->flags == f_start) || (opt->flags & H_FLAG))
			disp_des_usage(opt, ((opt->flags & H_FLAG) ? NULL : opt->argvs[i]));
		i++;
	}
}

void		check_des_args(t_des_opts *opt)
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
**	handle_des
**	This is useful:
**	https://wiki.openssl.org/index.php/Manual:Enc(1)
**
**	SUPER IMPORTANT NOTE!!! 'des' is aliased as des-ecb in ft_ssl_des. It's
**	really dumb that it diverges from openssl but that's life...
*/

void		handle_des(char *name, int argc, char **argvs)
{
	t_des_opts	des_opt;
	size_t		key_size;

	ft_bzero(&des_opt, sizeof(des_opt));
	name = (ft_strcmp(name, "des") == 0) ? "des-ecb" : name;
	name = (ft_strcmp(name, "des3") == 0) ? "des3-cbc" : name;
	des_opt.cmd_name = name;
	des_opt.argc = argc;
	des_opt.argvs = argvs;
	des_opt.flags |= (ft_strstr(name, "-ecb")) ? ECB : 0;
	des_opt.flags |= ((des_opt.flags & ECB) == 0) ? CBC : 0;
	des_opt.flags |= (ft_strstr(name, "des3")) ? DES3 : 0;
	parse_des_args(&des_opt);
	check_des_args(&des_opt);
	key_size = (des_opt.flags & DES3) ? 48 : 16;
	process_key_and_iv(&des_opt, key_size);
	des_initializer(&des_opt);
}
