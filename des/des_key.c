/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_key.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/04 12:56:47 by nmei              #+#    #+#             */
/*   Updated: 2018/02/10 18:32:57 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <readpassphrase.h>
#include <libft.h>
#include <des.h>

/*
**  Permuted Choice 1
*/

const int	g_des_pc1[56] =
{
	57, 49, 41, 33, 25, 17, 9,
	1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27,
	19, 11, 3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29,
	21, 13, 5, 28, 20, 12, 4
};

/*
**  Permuted Choice 2
*/

const int	g_des_pc2[48] =
{
	14, 17, 11, 24, 1, 5,
	3, 28, 15, 6, 21, 10,
	23, 19, 12, 4, 26, 8,
	16, 7, 27, 20, 13, 2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

/*
**	standardize_hex()
**	Given a 'hex_str' and a *desired* 'hex_len', create a new str that contains
**	the number of characters in 'hex' and pad with '0' ascii chars if it is not
**	already at the desired 'hex_len'.
*/

char		*standardize_hex(char *hex_str, size_t hex_len, int mode)
{
	char	*new;
	size_t	in_hex_len;
	char	*type;

	if (mode == 0)
		type = "key [-k]";
	else
		type = "initialization vector [-v]";
	new = (char *)malloc((hex_len + 1) * sizeof(char));
	ft_memset(new, '0', hex_len);
	new[hex_len] = '\0';
	in_hex_len = ft_strlen(hex_str);
	if (in_hex_len > hex_len)
		ft_dprintf(2, "Warning! Your %s is longer than %d hex characters, "
					"it will be truncated!\n", type, hex_len);
	else if (in_hex_len < hex_len)
		ft_dprintf(2, "Warning! Your %s is shorter than %d hex characters, "
					"it will be padded with '0' ascii characters but will "
					"result in weaker encryption!!!\n", type, hex_len);
	ft_strncpy(new, hex_str, (hex_len < in_hex_len) ? hex_len : in_hex_len);
	free(hex_str);
	return (new);
}

/*
**	process_key_and_iv()
**
**	Check if we've gotten a key. If not, ask for one. Either way, this
**	function will standardize the key it recieves.
**
**	If we're in CBC mode, then we also need to get an initialization vector.
**	Of course, we'll also need to standardize *that* too.
*/

void		process_key_and_iv(t_des_opts *des_opt, size_t key_size)
{
	if ((des_opt->flags & K_FLAG) == 0)
	{
		des_opt->des_key = ft_strnew(key_size + 1);
		readpassphrase("Please enter a des key (in hex): ",
						des_opt->des_key, key_size + 1, RPP_ECHO_OFF);
	}
	des_opt->des_key = standardize_hex(des_opt->des_key, key_size, 0);
	if ((des_opt->flags & CBC))
	{
		if ((des_opt->flags & V_FLAG) == 0)
		{
			des_opt->des_ivec = ft_strnew(17);
			readpassphrase("Please enter an initialization vector (in hex): ",
							des_opt->des_ivec, 17, RPP_ECHO_OFF);
		}
		des_opt->des_ivec = standardize_hex(des_opt->des_ivec, 16, 1);
	}
}

/*
**	rot_subkey()
**	Using the example from:
**	http://page.math.tu-berlin.de/~kant/teaching/hess/krypto-ws2006/des.htm
**
**	Given some 64-bit key:
**	1															   64
**	^															   ^
**	0001001100110100010101110111100110011011101111001101111111110001
**
**	After applying g_des_pc1 we get:
**	1						   28		   				   56	   64
**	^						   ^						   ^       ^
**	1111000011001100101010101111010101010110011001111000111100000000
**
**	We need to split this 56-bit version of the key into 28-bit halves...
**
**	To get the right part is easy so we'll describe that first, just
**	bit shift to the left 28 bits.
**
**	Right (key_56b << 28):
**	1						   28								   64
**  ^						   ^								   ^
**	0101010101100110011110001111000000000000000000000000000000000000
**	^						   ^
**	29						   56
**
**	Getting the left part is a 'bit' trickier (haha >_>). If we shift our
**	original key by 36 bits right we'll get:
**
**	key_56b >> 36:
**	0000000000000000000000000000000000001111000011001100101010101111
**
**	Shifting left by 36 bits again will get us our desired:
**
**	Left ((key_56b >> 36) << 36):
**	1111000011001100101010101111000000000000000000000000000000000000
**
**	======================== Rotations =============================
**
**	To do our rotations we can use the above trick. If we 'OR' each half
**	with itself bitshifted right 28 bits we get:
**
**	After (left | (left >> 28)):
**	1111000011001100101010101111111100001100110010101010111100000000
**
**	We can then perform our leftward rotation. Whether it be 1 or 2 bits left
**	doesn't matter.
**
**	After ((left | (left >> 28)) << 2):
**	1100001100110010101010111111110000110011001010101011110000000000
**
**	Finally do our (>> 36) (<< 36) ops to get:
**
**	After (((left | (left >> 28)) << 2) >> 36) << 36:
**	1100001100110010101010111111000000000000000000000000000000000000
**
**	To put our key halves back together again we can just bitshift the right
**	half right by 28 bits and 'OR' with the left half
**
**	EX:
**
**	Right >> 28:
**	0000000000000000000000000000010101011001100111100011110100000000
**	Left:
**	1100001100110010101010111111000000000000000000000000000000000000
*/

uint64_t	rot_subkey(uint64_t key_56b, int l_rots)
{
	uint64_t	left;
	uint64_t	right;

	left = (key_56b >> 36) << 36;
	right = key_56b << 28;
	left = (((left | (left >> 28)) << l_rots) >> 36) << 36;
	right = (((right | (right >> 28)) << l_rots) >> 36) << 36;
	return (left | (right >> 28));
}

/*
**	create_des_subkeys()
**	Described in detail here:
**	http://page.math.tu-berlin.de/~kant/teaching/hess/krypto-ws2006/des.htm
**
**	To create the 16 subkeys specified in des:
**	1) Permute the 64 bit key with pc1 to get a 56 bit permutation
**	2) Rotate the left and right halves (28 bits each) of the 56 bit permutation
**	   according to the rotation schedule (shifts are specified as left shifts
**	   where each bit is moved one place to the left)
**	3) Permute the shifted 56 bit key with pc2 to obtain a final subkey that is
**	   48 bits.
*/

uint64_t	*create_des_subkeys(char *key_64b)
{
	const static int	rots[16] =
	{1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};
	uint64_t			curr_key;
	uint64_t			*subkeys;
	int					i;

	if (key_64b && (subkeys = (uint64_t *)malloc(16 * sizeof(uint64_t))))
	{
		curr_key = permute(des_htob(key_64b), g_des_pc1, 56);
		i = -1;
		while (++i < 16)
		{
			curr_key = rot_subkey(curr_key, rots[i]);
			subkeys[i] = permute(curr_key, g_des_pc2, 48);
		}
		return (subkeys);
	}
	return (NULL);
}

/*
**	create_des3_subkeys()
**	Instead of a 64-bit key now we have 3 keys so 3 * 64 = 192 bits!
**	Described here:
**	https://en.wikipedia.org/wiki/Triple_DES
**
**	For encryption:
**	ciphertext = EK3(DK2(EK1(plaintext)))
**
**	For decryption:
**	plaintext = DK1(EK2(DK3(ciphertext)))
*/

uint64_t	**create_des3_subkeys(t_des_opts *opts, char *key_192b)
{
	uint64_t	**subkeys_3;
	char		*k1;
	char		*k2;
	char		*k3;

	if (key_192b && (subkeys_3 = (uint64_t **)malloc(3 * sizeof(uint64_t *))))
	{
		k1 = (opts->flags & E_FLAG) ? key_192b : key_192b + 32;
		k2 = key_192b + 16;
		k3 = (opts->flags & E_FLAG) ? key_192b + 32 : key_192b;
		subkeys_3[0] = create_des_subkeys(k1);
		subkeys_3[1] = create_des_subkeys(k2);
		subkeys_3[2] = create_des_subkeys(k3);
		return (subkeys_3);
	}
	return (NULL);
}
