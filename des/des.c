/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/08 10:43:16 by nmei              #+#    #+#             */
/*   Updated: 2018/02/10 21:48:55 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <base64.h>
#include <des.h>
#include <libft.h>

/*
**	des()
**
**	Implements both ebc and cbc in one function!
**
**	For encryption CBC requires us to XOR the input block with our
**	initialization vector but afterwards we XOR subsequent inputs with the
**	output of the last block after the DES process. Decryption is the reverse.
**
**	See these nice pictures for a much better pictoral description of CBC:
**	https://en.wikipedia.org/wiki/
**	Block_cipher_mode_of_operation#Cipher_Block_Chaining_(CBC)
*/

uint8_t	*des(t_des_opts *opts, uint8_t *input, ssize_t len, uint64_t *subkeys)
{
	int			i;
	uint64_t	iv;
	uint64_t	block;
	uint64_t	des_block;
	uint8_t		*output;

	output = (uint8_t *)malloc((len + 1) * sizeof(uint8_t));
	output[len] = '\0';
	iv = (opts->flags & CBC) ? des_htob(opts->des_ivec) : 0;
	i = 0;
	while (i < (int)len)
	{
		block = pack_block(input + i);
		block ^= ((opts->flags & CBC) && (opts->flags & E_FLAG)) ? iv : 0;
		des_block = des_process_block(block, subkeys, (opts->flags & E_FLAG));
		des_block ^= ((opts->flags & CBC) && (opts->flags & D_FLAG)) ? iv : 0;
		iv = (opts->flags & E_FLAG) ? des_block : block;
		unpack_block(des_block, output + i);
		i += 8;
	}
	return (output);
}

/*
**	des3()
**
**	Also implements both ebc and cbc in one function!
**
**	Really the only difference between des and triple des is that
**	triple des goes through 3 rounds of des processing with (hopefully) 3
**	unique keys.
**
**	To encrypt with triple des we:
**	encrypt with k1 --> decrypt with k2 --> and encrypt with k3
**
**	To decrypt we:
**	decrypt with k3 --> encrypt with k2 --> and decrypt with k1
*/

uint8_t	*des3(t_des_opts *opts, uint8_t *input, ssize_t len, uint64_t **sks_3)
{
	int			i;
	uint64_t	iv;
	uint64_t	blk;
	uint64_t	des_blk;
	uint8_t		*output;

	output = (uint8_t *)malloc((len + 1) * sizeof(uint8_t));
	output[len] = '\0';
	iv = (opts->flags & CBC) ? des_htob(opts->des_ivec) : 0;
	i = 0;
	while (i < (int)len)
	{
		blk = pack_block(input + i);
		blk ^= ((opts->flags & CBC) && (opts->flags & E_FLAG)) ? iv : 0;
		des_blk = des_process_block(blk, sks_3[0], (opts->flags & E_FLAG));
		blk = des_process_block(des_blk, sks_3[1], !(opts->flags & E_FLAG));
		des_blk = des_process_block(blk, sks_3[2], (opts->flags & E_FLAG));
		des_blk ^= ((opts->flags & CBC) && (opts->flags & D_FLAG)) ? iv : 0;
		iv = (opts->flags & E_FLAG) ? des_blk : pack_block(input + i);
		unpack_block(des_blk, output + i);
		i += 8;
	}
	return (output);
}

/*
**	des_choice()
**	Function that will run the correct des function.
*/

uint8_t	*des_choice(t_des_opts *opts, uint8_t *input, ssize_t len)
{
	int			i;
	uint64_t	*subkeys;
	uint64_t	**subkeys_3;
	uint8_t		*output;

	if (opts->flags & DES3)
	{
		subkeys_3 = create_des3_subkeys(opts, opts->des_key);
		output = des3(opts, input, len, subkeys_3);
		i = -1;
		while (++i < 3)
			free(subkeys_3[i]);
		free(subkeys_3);
	}
	else
	{
		subkeys = create_des_subkeys(opts->des_key);
		output = des(opts, input, len, subkeys);
		free(subkeys);
	}
	if (opts->flags & CBC)
		free(opts->des_ivec);
	free(opts->des_key);
	return (output);
}

void	des_print_encrypted(t_des_opts *opts, uint8_t *output, ssize_t len)
{
	int		i;

	if (opts->flags & A_FLAG)
	{
		i = 0;
		while (i < (int)len)
		{
			ft_dprintf(opts->out_fd, "%.64s\n", output + i);
			i += 64;
		}
	}
	else
	{
		write(opts->out_fd, output, len);
	}
}

/*
**	des_initializer()
**	Prepares for DES encryption or decryption.
**
**	Need to:
**	1) Read in our cipher or plain text
**	2) If we have the [-a] flag, our input is in base64 so decode it
**	3) If we're doing encryption then we need pad our input with PKCS#7
**	   Note: only if NP_FLAG == 0
**	4) run our des choice to produce our output
**	5) If we're doing decryption then we need to unpad our output
**	   Note: only if NP_FLAG == 0
**	5) If we have the [-a] flag, we need to encode our output into base64
**	6) Print our output to the desired output fd and clean things up.
**	   NOTE: we use the 'write()' function in cases where someone evil can
**	   potentially create output like: "Test\0x00\0x00\0x00I'm Hidden",
**	   then we want to write out all bytes even the 'hidden ones'.
**	   My ft_printf unfortunately faceplants on this task...
*/

void	des_initializer(t_des_opts *opts)
{
	char		*tmp;
	uint8_t		*input;
	uint8_t		*output;
	ssize_t		len;

	len = ft_read_all(opts->in_fd, &tmp);
	input = (uint8_t *)ft_memdup(tmp, len);
	free(tmp);
	if ((opts->flags & A_FLAG) && (opts->flags & D_FLAG))
		input = (uint8_t *)decode_base64((char *)input, len, &len);
	if ((opts->flags & E_FLAG))
		des_pad_input(opts, &input, &len);
	output = des_choice(opts, input, len);
	if ((opts->flags & D_FLAG))
		des_unpad_output(opts, output, &len);
	if ((opts->flags & A_FLAG) && (opts->flags & E_FLAG))
		output = (uint8_t *)encode_base64((char *)output, len, &len);
	if (opts->flags & E_FLAG)
		des_print_encrypted(opts, output, len);
	else
		write(opts->out_fd, output, len);
	free(input);
	free(output);
	close(opts->out_fd);
}
