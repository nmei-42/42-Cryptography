/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_algorithm.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/03 15:05:54 by nmei              #+#    #+#             */
/*   Updated: 2018/02/10 18:05:04 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <des.h>

/*
**  These tables for DES are obtained from:
**  "https://csrc.nist.gov/csrc/media/publications/fips/46/3/archive/1999-10-25/
**  documents/fips46-3.pdf"
**
**  Can also be obtained from:
**  https://en.wikipedia.org/wiki/DES_supplementary_material
*/

/*
**  Initial Permutation
*/

static const int	g_des_ip[64] =
{
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7
};

/*
**  Final Permutation
*/

static const int	g_des_fp[64] =
{
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41, 9, 49, 17, 57, 25
};

static const int	g_des_expansion[48] =
{
	32, 1, 2, 3, 4, 5,
	4, 5, 6, 7, 8, 9,
	8, 9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32, 1
};

static const int	g_des_pbox[32] =
{
	16, 7, 20, 21, 29, 12, 28, 17,
	1, 15, 23, 26, 5, 18, 31, 10,
	2, 8, 24, 14, 32, 27, 3, 9,
	19, 13, 30, 6, 22, 11, 4, 25
};

static const int	g_des_sboxes[8][4][16] =
{
	{
		{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
		{0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
		{4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
		{15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
	},
	{
		{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
		{3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
		{0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
		{13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
	},
	{
		{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
		{13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
		{13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
		{1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
	},
	{
		{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
		{13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
		{10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
		{3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
	},
	{
		{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
		{14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
		{4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
		{11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
	},
	{
		{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
		{10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
		{9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
		{4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
	},
	{
		{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
		{13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
		{1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
		{6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
	},
	{
		{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
		{1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
		{7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
		{2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
	}
};

/*
**	permute()
**	Applies des permutation or expansion operations to data.
**
**	To get the i-th bit of n:
**	((n >> i) & 1)
**	To set the i-th bit of n to k:
**	n |= ( k << i);
**
**	One wrinkle is that the permute vectors described are all in
**	big endian. So the most significant bit (left most one) is numbered 1
**	while the least significant one is numbered 64 (right most bit).
**	See: https://en.wikipedia.org/wiki/DES_supplementary_material
**
**	1															   64
**	^															   ^
**	0001001100110100010101110111100110011011101111001101111111110001
**
**	So if the i-th element of our permute vector calls for the 64th bit then
**	our bit shift needs to be == 0 (i.e. '64 - permute_vector[i]').
**
**	Because our 'i' starts at 0 and goes to size - 1. To put our new values
**	in the correct spot we need to add 1 to our i. Put another way, when i is
**	0 our bitshift for our new_val should be 63.
*/

uint64_t		permute(uint64_t block, const int *permute_vec, int size)
{
	uint64_t	new_block;
	uint64_t	new_val;
	int			i;

	new_block = 0;
	i = -1;
	while (++i < size)
	{
		new_val = ((block >> (64 - permute_vec[i])) & 1);
		if (new_val)
			new_block |= (new_val << (64 - (i + 1)));
	}
	return (new_block);
}

/*
**	feistel()
**	Apply the feistel function to a given half_block.
**
**	1) We first take our half block (32 bits) and perform expansion on it
**	   turning it to 48 bits.
**	2) We then XOR our expanded half block with the 48-bit subkey
**	3) Now, we divide our 48-bit block into 6-bit pieces that are
**	   used as lookups with our S-boxes. The S-boxes will return 4-bit
**	   non-linearly transformed outputs.
**	   a) To get a lookup from our s-box we need to get each bit on the edges of
**		  our current 6-bit piece AND we need to get the inner 4 bits.
**
**	   Ex1: 100101
**	   Our outer bits are 1 and 1 which if read as a 2-bit binary would be 3
**	   Our inner bits are 0010 which if read as a 4-bit binary would be 2
**	   Our s-box lookup if our i = 0 is 8 which in 4-bits is 1000
**
**	   Ex2: 101000
**	   Our outer bits are 1 and 0 which if read as a 2-bit binary would be 2
**	   Our inner bits are 0100 which if read as a 4-bit binary would be 4
**	   Our s-box lookup if our i = 1 is 10 which in 4-bits is 1010
**
**	   b) We put each set of 4 bits from the most significant bit onwards
**		  until we've filled 32 bits again!
**
**	   Using the above 2 examples our result after 2 blocks of 6-bits to 4-bits
**	   with s-boxes would look like:
**
**	   1							  32							  64
**	   ^							  ^								  ^
**	   1000101000000000000000000000000000000000000000000000000000000000
**
**	4) We then permute our 32 outputs from the S-boxes with our P-box and
**	   return the result!
*/

static uint64_t	feistel(uint64_t half_block, uint64_t subkey)
{
	int			i;
	uint64_t	result;
	uint8_t		inner;
	uint8_t		outer;

	result = 0;
	half_block = permute(half_block, g_des_expansion, 48);
	half_block ^= subkey;
	i = -1;
	while (++i < 8)
	{
		outer = (((half_block >> (64 - (6 * i + 1))) & 1) << 1) |
				((half_block >> (64 - (6 * i + 6)) & 1));
		inner = (half_block << (6 * i + 1)) >> 60;
		result |= (uint64_t)g_des_sboxes[i][outer][inner] << (60 - (4 * i));
	}
	result = permute(result, g_des_pbox, 32);
	return (result);
}

/*
**	des_round()
**	Each round of des goes as follows (where n is the current round):
**
**	1) L(n) = R(n - 1);
**	2) R(n) = L(n - 1) XOR feistel(R(n - 1), subkey(n));
**
**	Step 2 can also be written as:
**	2*) R(n) = L(n - 1) XOR feistel(L(n), subkey(n));
**
**	So:
**	1) Set our 'new left' to be the previous round's right (message << 32)
**	2) Set our 'new right' to be the previous round's left (message >> 32)
**	3) We XOR our 'new right' with the result of the feistel function applied to
**	   our previous round's right (i.e. our current round's left).
**	   Note: We need to bit shift our feistel result right by 32 bits for it to
**	   be XOR'able with our 'new right' which was the previous round's left
**	   (message >> 32).
*/

static uint64_t	des_round(uint64_t block_64b, uint64_t subkey)
{
	uint64_t	left;
	uint64_t	right;

	left = block_64b << 32;
	right = block_64b >> 32;
	right ^= (feistel(left, subkey) >> 32);
	return ((left | right));
}

/*
**	des_process_block()
**
**	1) After we get our 64-bit message block we need to apply the initial
**	permutation to it.
**	2) Then we go through 16 rounds of processing message halves with the
**	   feistel function.
**	   Note: If we apply our subkeys in reverse order we get decryption! So
**	   mode 1 is when we want to encrypt and mode 0 is when we want to decrypt
**	3) At the end of our 16th round we need to swap our left and right halves
**	4) Finally we apply the final permutation.
*/

uint64_t		des_process_block(uint64_t block_64b, uint64_t *subkeys,
									int mode)
{
	int			i;

	block_64b = permute(block_64b, g_des_ip, 64);
	i = -1;
	while (++i < 16)
	{
		if (mode == 1)
			block_64b = des_round(block_64b, subkeys[i]);
		else
			block_64b = des_round(block_64b, subkeys[15 - i]);
	}
	block_64b = ((block_64b << 32) | (block_64b >> 32));
	block_64b = permute(block_64b, g_des_fp, 64);
	return (block_64b);
}
