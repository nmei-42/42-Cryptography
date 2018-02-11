/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/05 14:37:09 by nmei              #+#    #+#             */
/*   Updated: 2018/02/10 18:05:38 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <libft.h>
#include <des.h>

/*
**	des_htob()
**	hex (str) to bytes for a 64-bit key string.
**
**	Because our keys will be standardized to have '0' padding, we can
**	confidently just iterate through 16 times to get the 16 chars that make up
**	the 64-bit key string.
*/

uint64_t	des_htob(char *key)
{
	uint64_t	result;
	int			i;
	int			dig;
	char		c;

	result = 0;
	i = 0;
	while (i < 16)
	{
		c = key[i++];
		if (c >= 'A' && c <= 'F')
			dig = c - 'A' + 10;
		else if (c >= 'a' && c <= 'f')
			dig = c - 'a' + 10;
		else if (c >= '0' && c <= '9')
			dig = c - '0';
		else
		{
			ft_dprintf(2, "Error! Non hex character '%c' encountered!", c);
			exit(0);
		}
		result = result * 16 + dig;
	}
	return (result);
}

/*
**	pack_block()
**	Our des algorithm works on 64-bit blocks so we need to pack our 8-bit
**	characters into 64-bit block format for things to work.
*/

uint64_t	pack_block(uint8_t *str)
{
	int			i;
	uint64_t	block;

	block = 0;
	i = -1;
	while (++i < 8)
		block |= ((uint64_t)str[i]) << (8 * (8 - (i + 1)));
	return (block);
}

void		unpack_block(uint64_t block, uint8_t *out_str)
{
	int		i;

	i = -1;
	while (++i < 8)
		out_str[i] = (char)(block >> (8 * (8 - (i + 1))));
}

/*
**	des_pad_input;
**	wow openssl uses PKCS#7 padding so even if we have full blocks of input
**	openssl will add another block of padding.
**
**	See: https://crypto.stackexchange.com/questions/12621/
**
**	We'll also use our des_pad_input function to check if our input is a
**	multiple of block length if we're using the '-nopad' (NP_FLAG) option.
*/

void		des_pad_input(t_des_opts *opts, uint8_t **input, ssize_t *len)
{
	ssize_t remaind;
	ssize_t	new_len;
	uint8_t	pad_char;
	uint8_t	*new_input;

	remaind = *len % 8;
	if (!(opts->flags & NP_FLAG))
	{
		pad_char = (uint8_t)(8 - remaind);
		new_len = 8 * (*len / 8 + 1);
		new_input = (uint8_t *)malloc((new_len + 1) * sizeof(uint8_t));
		new_input[new_len] = '\0';
		ft_memset(new_input, pad_char, new_len);
		ft_memcpy(new_input, *input, *len);
		free(*input);
		*len = new_len;
		*input = new_input;
	}
	else if (remaind)
	{
		ft_dprintf(2, "%s: Error! Data not a multiple of block length!\n",
						opts->cmd_name);
		exit(1);
	}
}

/*
**	des_unpad_output()
**	Check if our decrypted output plaintext has padding. If so then modify
**	the *len so that it will reflect only non-pad characters.
*/

void		des_unpad_output(t_des_opts *opts, uint8_t *output, ssize_t *len)
{
	int		valid_pad;
	uint8_t	last_char;
	uint8_t	i;

	valid_pad = 1;
	last_char = output[*len - 1];
	if (last_char <= 8)
	{
		i = 0;
		while (i < last_char)
		{
			if (valid_pad)
				valid_pad = (output[*len - 1 - i++] == last_char) ? 1 : 0;
		}
		if (valid_pad && !(opts->flags & NP_FLAG))
			*len -= last_char;
	}
}
