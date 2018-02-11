/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base64_encode.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/03 12:36:53 by nmei              #+#    #+#             */
/*   Updated: 2018/02/10 17:52:11 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <libft.h>
#include <base64.h>
#define E_BLOCK(x,y,z) (((x)<<16L)|((y)<<8L)|(z))

/*
**	binary_to_ascii()
**
**	Given an unsigned char (i.e. uint8_t implicitly cast from uint32_t)
**	truncate the character from 8 bit char to 6 bit 'binary'
**	unsigned char (also uint8_t) and return the corresponding base64 character.
**
**	Example:
**	Say we had the char 'M' (77 or 0x4D or 01001101) the 6 most significant
**	bits are thus 010011 (or 0x13 or 19). We use the new value '19' to perform
**	our binary to ascii lookup.
**
**	Binary to Ascii lookup:
**	b_to_a[19] : 'T'
**
**	Note: We '&' mask 'binary' with 0x3f (63) since the max valid index is 63.
**		  This plays the role of truncating to 6-bits AND protects our lookup.
*/

uint8_t		binary_to_ascii(uint8_t binary)
{
	static const uint8_t b_to_a[65] =
	{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

	return (b_to_a[(binary & 0x3f)]);
}

/*
**	base64_encoder()
**	Given a 'src' uchar array and a 'src_length', generate a base 64 encoded
**	version of 'src' and store it in 'dst' (and '\0' terminate it).
**	Returns number of characters written to 'dst' (NOT including '\0').
**
**	If 'src_len' is not a multiple of 3 then encode_base64() will write an
**	ending base64 block in 'dst' (i.e. '='' or '=='')
**
**	Base 64 encoding works by taking 3 8-bit uchars and putting them into a 24
**	bit "block". This "block" is then reinterpreted as "4" 6-bit uchars
**	these 6-bit uchars take on a value from 0 to 63 inclusive which can then
**	be used in a lookup table to get the base64 encoded value.
*/

ssize_t		base64_encoder(uint8_t *dst, uint8_t *src, size_t src_len)
{
	uint32_t	block;
	ssize_t		ret;

	ret = 0;
	while (src_len)
	{
		if (src_len >= 3)
			block = E_BLOCK((uint32_t)src[0], (uint32_t)src[1],
							(uint32_t)src[2]);
		else
		{
			block = (((uint32_t)src[0]) << 16L);
			block |= (src_len == 2) ? (((uint32_t)src[1]) << 8L) : 0;
		}
		*(dst++) = binary_to_ascii(block >> 18L);
		*(dst++) = binary_to_ascii(block >> 12L);
		*(dst++) = (src_len == 1) ? '=' : binary_to_ascii(block >> 6L);
		*(dst++) = (!(src_len >= 3)) ? '=' : binary_to_ascii(block);
		src_len = (src_len >= 3) ? src_len - 3 : 0;
		ret += 4;
		src += 3;
	}
	*dst = '\0';
	return (ret);
}

void		base64_print_encrypted(int fd, char *encrypted, ssize_t in_len)
{
	int		i;

	i = 0;
	while (i < (int)in_len)
	{
		ft_dprintf(fd, "%.64s\n", encrypted + i);
		i += 64;
	}
}

/*
**	encode_base64()
**	Given plaintext input and input length, return base64 cipher output.
*/

char		*encode_base64(char *input, ssize_t in_len, ssize_t *out_len)
{
	size_t	enc_len;
	char	*output;

	enc_len = (in_len / 3) * 4;
	output = (char *)malloc((enc_len + 1) * sizeof(char));
	*out_len = base64_encoder((uint8_t *)output, (uint8_t *)input, in_len);
	free(input);
	return (output);
}
