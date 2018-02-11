/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/29 22:12:35 by nmei              #+#    #+#             */
/*   Updated: 2018/02/10 20:16:58 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DES_H
# define DES_H
# include <unistd.h>
# include <stddef.h>
# include <stdint.h>
# define E_FLAG	1
# define D_FLAG	2
# define I_FLAG	4
# define O_FLAG	8
# define H_FLAG 16
# define A_FLAG 32
# define K_FLAG 64
# define V_FLAG 128
# define NP_FLAG 256
# define ECB 512
# define CBC 1024
# define DES3 2048

typedef struct	s_des_opts
{
	char		*cmd_name;
	char		**argvs;
	int			argc;
	int			flags;
	int			in_fd;
	int			out_fd;
	char		*ipath;
	char		*opath;
	char		*des_key;
	char		*des_ivec;
}				t_des_opts;

/*
**	des_utils.c
*/
uint64_t		des_htob(char *key);
uint64_t		pack_block(uint8_t *str);
void			unpack_block(uint64_t block, uint8_t *out_str);
void			des_pad_input(t_des_opts *opts, uint8_t **input, ssize_t *len);
void			des_unpad_output(t_des_opts *opts, uint8_t *output,
								ssize_t *len);

/*
**	des_algorithm.c
*/
uint64_t		permute(uint64_t block, const int *permute_vec, int size);
uint64_t		des_process_block(uint64_t block_64b, uint64_t *subkeys,
									int mode);
/*
**	des_key.c
*/
void			process_key_and_iv(t_des_opts *des_opt, size_t key_size);
uint64_t		*create_des_subkeys(char *key_64b);
uint64_t		**create_des3_subkeys(t_des_opts *opts, char *key_192b);

/*
**	des.c
*/
void			des_initializer(t_des_opts *opts);

#endif
