/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ssl.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/29 22:12:01 by nmei              #+#    #+#             */
/*   Updated: 2018/02/10 20:16:45 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_SSL_H
# define FT_SSL_H
# include <stddef.h>

typedef enum
{
	STANDARD,
	MESSAGE_DIGEST,
	CIPHER
}	t_cmd_type;

typedef	struct	s_cmd_lookup
{
	char		*name;
	void		(*cmd_func)(char *, int, char **);
	t_cmd_type	cmd_type;
}				t_cmd_lookup;

/*
**	base64_handler.c
*/

void			handle_base64(char *name, int argc, char **argvs);

/*
**	des_handler.c
*/

void			handle_des(char *name, int argc, char **argvs);

#endif
