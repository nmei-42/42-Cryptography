/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base64_decode.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/29 22:56:14 by nmei              #+#    #+#             */
/*   Updated: 2018/02/10 19:21:03 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <libft.h>
#include <base64.h>
#define D_BLOCK(w,x,y,z) (((w)<<18L)|((x)<<12L)|((y)<<6L)|(z))
#define IS_ALPHA(x) (((x)>='a'&&(x)<='z')||((x)>='A'&&(x)<='Z'))
#define IS_NUMPS(y)	(((y)>='0'&&(y)<='9')||((y)=='+'||(y)=='/'||(y)=='='))
#define SPA(x) ((x)=='\t'||(x)=='\n'||(x)=='\v'||(x)=='\f'||(x)=='\r'||(x)==' ')
#define NAPP 0xFF

/*
**	ascii_to_binary()
**
**	Given an ascii character (in base64) what is the corresponding binary value?
**
**	Example:
**	Say we had the base64 char 'T' (84) we know that the b_to_a lookup that
**	gave 'T' (see above) was 19 (or 0x13 or 010011).
**
**	So, we just need to make a lookup table where index 84 yields 0x13!
**
**	Another example:
**	Say we had the base64 char 'z' (122) we know that the b_to_a lookup that
**	gave 'z' was 51 (or 0x33 or 110011).
**
**	Index 122 should yield 0x33!
**
**	We only care about A-Z, a-z, 0-9, +, / characters. All other values will be
**	set as Not APPlicable or 0xFF (255) which is impossible for a 6-bit value
**	to reach (6-bit values max out at 0x3F).
**
**	Note: It is possible for uint8_t to reach a max value of 0xFF so we need
**	to check that our ascii input is in the ascii range before performing
**	our lookup.
*/

uint8_t		ascii_to_binary(uint8_t ascii)
{
	static const uint8_t a_to_b[128] =
	{NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP,
	NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP,
	NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP,
	NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, NAPP, 0x3E, NAPP, NAPP, NAPP, 0x3F,
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, NAPP, NAPP,
	NAPP, NAPP, NAPP, NAPP, NAPP, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
	0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
	0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, NAPP, NAPP, NAPP, NAPP, NAPP,
	NAPP, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24,
	0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
	0x31, 0x32, 0x33, NAPP, NAPP, NAPP, NAPP, NAPP};

	if (ascii >= 128)
		return (NAPP);
	else
		return (a_to_b[ascii]);
}

/*
**	ascii_to_binary_conversion()
**	Takes in 4 'src' base64 chars and performs the ascii to binary lookup
**	and puts them into a temporary 24-bit "block". Also advances 'src' pointer
**	by 4.
**
**	If we encounter '=' symbol then we fill parts of the 24-bit block with
**	0 values instead of from 'src'.
*/

void		ascii_to_binary_conversion(t_block *tmp_block, int *pad_len,
										uint8_t **src)
{
	*pad_len = 0;
	tmp_block->a = ascii_to_binary((*src)[0]);
	tmp_block->b = ascii_to_binary((*src)[1]);
	if ((*src)[3] == '=')
		*pad_len = ((*src)[2] == '=') ? 2 : 1;
	tmp_block->c = (*pad_len < 2) ? ascii_to_binary((*src)[2]) : 0;
	tmp_block->d = (*pad_len < 1) ? ascii_to_binary((*src)[3]) : 0;
	*src += 4;
}

/*
**	base64_decoder()
**	Given a 'src' uchar array (encoded in base64) and a 'src_length', generate
**	a decoded version of 'src' and store it in 'dst' (and '\0' terminate it).
**	Returns number of characters written to 'dst' (NOT including '\0').
**
**	Base 64 decoding works by taking "4" 8-bit 'base64' uchars and reverse
**	converting them (see: ascii_to_binary()) back to 6-bit values and putting
**	them into a 24 bit "block". This "block" is the reinterpreted as "3" 8-bit
**	uchars which undoes the base64 encoding.
**
**	Note 1: The 'src' string needs to be free of any non-base64 chars!
**	Note 2: If src/src_len is not a multiple of 4, there will be very odd
*/

ssize_t		base64_decoder(uint8_t *dst, uint8_t *src, size_t src_len)
{
	t_block		tmp_block;
	uint32_t	block;
	ssize_t		ret;
	int			pad_len;
	int			i;

	i = 0;
	ret = 0;
	while (i + 4 <= (int)src_len)
	{
		ascii_to_binary_conversion(&tmp_block, &pad_len, &src);
		i += 4;
		block = D_BLOCK((uint32_t)tmp_block.a, (uint32_t)tmp_block.b,
						(uint32_t)tmp_block.c, (uint32_t)tmp_block.d);
		*(dst++) = (uint8_t)(block >> 16L) & 0xFF;
		if (pad_len < 2)
			*(dst++) = (uint8_t)(block >> 8L) & 0xFF;
		if (pad_len < 1)
			*(dst++) = (uint8_t)(block) & 0xFF;
		ret += 3 - pad_len;
	}
	*dst = '\0';
	return (ret);
}

/*
**	remove_spacers()
**	Function that will remove 'space' characters from our base64 input string
**	buffer.
**
**	The function will directly modify the input buffer string so that all
**	non-space characters will be contiguous with one another. The length of the
**	continuous non-space characters will overwrite the existing len provided to
**	the function. Extra space in the buffer will be set to '\0'.
**
**	Example input: "I am  a   string that   has lots of	\tspacers\n" (len = 45)
**
**	Example output: "Iamastringthathaslotsofspacers" (and 15 '\0's) (len = 30)
**
**	We want to do this in O(n) time so:
**	1) Iterate through our input string.
**	   a) If we encounter a spacing character we set it to '\0', increase our
**		  offset counter by 1 (this allows us to overwrite space characters
**		  with non-space ones) and go evaluate the next character.
**	   b) If we encounter a normal character just copy buff[i] to itself
**	      (i.e. buff[i] = buff[i]). However if we've previously encountered
**		  a spacing character then we need to offset our index by one so that
**		  our non-space characters will be shifted to overwrite the space char.
**	   c) We also need to check if our input chars are valid base64
**		  (i.e. between 'a'-'z', 'A'-'Z', '0'-'9' or '+' or '\' or '=')
**	   d) If our offset is greater than 0 then we can start overwriting 'i'th
**		  position chars with '\0's.
**	2) Our new len will be the length of the buffer - the number of space chars
**	   encountered.
*/

void		remove_spacers(char *buff, ssize_t *len)
{
	int	i;
	int	offset;

	i = 0;
	offset = 0;
	while (i < *len)
	{
		if (i >= *len)
			break ;
		if (SPA(buff[i]))
		{
			buff[i++] = '\0';
			offset++;
			continue ;
		}
		buff[i - offset] = buff[i];
		if (!(IS_ALPHA(buff[i]) || IS_NUMPS(buff[i])))
		{
			ft_dprintf(2, "\n\nInvalid char [%c] in input stream!\n", buff[i]);
			exit(1);
		}
		buff[i] = (offset > 0) ? '\0' : buff[i];
		i++;
	}
	*len = i - offset;
}

/*
**	decode_base64()
**	Given base64 cipher input and input length, return plaintext output.
**
**	Basic idea: We want to decode our base64 ascii string back to plaintext. The
**	minimum example would be that we want to convert 4 base64 chars back to
**	3 plaintext chars. (4 * 6 bits = 3 * 8 bits)
**
**	Problem: Our input stream will likely include non-base64 characters.
**	Example offenders: ' ' or '\t' or '\n' or '\v' or '\f' or '\r'
**
**	Solution: We don't want to send these characters to our decode_base64()
**	function so we can remove them with the remove_spacers() function.
*/

char		*decode_base64(char *input, ssize_t in_len, ssize_t *out_len)
{
	size_t	dec_len;
	char	*decrypted;

	remove_spacers(input, &in_len);
	dec_len = (in_len / 4) * 3;
	decrypted = (char *)malloc((dec_len + 1) * sizeof(char));
	*out_len = base64_decoder((uint8_t *)decrypted, (uint8_t *)input, in_len);
	free(input);
	return (decrypted);
}
