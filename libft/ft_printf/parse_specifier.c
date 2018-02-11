/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_specifier.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/20 23:08:11 by nmei              #+#    #+#             */
/*   Updated: 2018/01/03 21:51:49 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>
#include <ft_printf.h>

/*
**	ft_printf must minimally handle s S p d D i o O u U x X c C
**	Setup a jump table where the given ascii (int) positions contain
**	addresses of function handlers.
*/

static void		setup_jump_table(t_jumptable j_table[128])
{
	j_table['%'] = &handle_escape;
	j_table['d'] = &handle_int;
	j_table['D'] = &handle_int;
	j_table['i'] = &handle_int;
	j_table['o'] = &handle_unsigned_int_oct;
	j_table['O'] = &handle_unsigned_int_oct;
	j_table['u'] = &handle_unsigned_int_dec;
	j_table['U'] = &handle_unsigned_int_dec;
	j_table['x'] = &handle_unsigned_int_hex;
	j_table['X'] = &handle_unsigned_int_hex;
	j_table['f'] = &handle_double;
	j_table['F'] = &handle_double;
	j_table['e'] = &handle_double_sci;
	j_table['E'] = &handle_double_sci;
	j_table['g'] = &handle_double_shortest;
	j_table['G'] = &handle_double_shortest;
	j_table['a'] = &handle_double_hex;
	j_table['A'] = &handle_double_hex;
	j_table['c'] = &handle_char;
	j_table['C'] = &handle_char;
	j_table['s'] = &handle_str;
	j_table['S'] = &handle_str;
	j_table['p'] = &handle_unsigned_int_hex;
	j_table['n'] = &handle_n;
}

/*
**	get_func() - Setup a jump table so that we can get the relevant function
**	for any given specifier that we find.
**
**	We make our jumptable static so we only need to set it once per printf call
**	and give it a size of 127 + 1 (the max value for an ascii char)
**
**	We need to protect our jump table lookup by making sure our char c is
**	an ascii.
*/

t_jumptable		get_handler_funct(char c)
{
	static t_jumptable	specifier_jt[128] = {NULL};

	if (specifier_jt['%'] == NULL)
		setup_jump_table(specifier_jt);
	if (ft_isascii(c))
		return (specifier_jt[(int)c]);
	else
		return (NULL);
}
