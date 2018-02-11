/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base64.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/03 12:39:54 by nmei              #+#    #+#             */
/*   Updated: 2018/02/10 20:16:57 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BASE64_H
# define BASE64_H
# include <unistd.h>
# include <stddef.h>
# include <stdint.h>
# define E_FLAG	1
# define D_FLAG	2
# define I_FLAG	4
# define O_FLAG	8
# define H_FLAG 16

typedef struct		s_b64_opts
{
	char			*cmd_name;
	char			**argvs;
	int				argc;
	int				flags;
	int				in_fd;
	int				out_fd;
	char			*ipath;
	char			*opath;
}					t_b64_opts;

typedef struct		s_block
{
	uint8_t			a;
	uint8_t			b;
	uint8_t			c;
	uint8_t			d;
}					t_block;

/*
**	base64_encode.c
*/

char				*encode_base64(char *input, ssize_t in_len,
									ssize_t *out_len);
ssize_t				base64_encoder(uint8_t *dst, uint8_t *src, size_t src_len);
void				base64_print_encrypted(int fd, char *encrypted,
									ssize_t in_len);

/*
**	base64_decode.c
*/

char				*decode_base64(char *input, ssize_t in_len,
									ssize_t *out_len);
ssize_t				base64_decoder(uint8_t *dst, uint8_t *src, size_t src_len);

#endif
