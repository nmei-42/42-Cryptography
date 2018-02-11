/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   int_handlers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/21 13:58:58 by nmei              #+#    #+#             */
/*   Updated: 2018/01/05 12:49:42 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <ft_printf.h>

/*
**	get_signed_int_arg()
**	If we're handling ints then we need to invoke va_arg. Because dealing
**	with different size possibilities is a pain, it's easiest to promote
**	everything to the max int size type.
**
**	We have to do a slight shuffle so that L_FLAG is checked before
**	H_FLAG and HH_FLAG because the D specifier should take precedence...
*/

static intmax_t	get_signed_int_arg(t_vfpf *p)
{
	if (p->flags & L_FLAG || p->specifier == 'D')
		return ((intmax_t)va_arg(p->args, signed long));
	else if (p->flags & HH_FLAG)
		return ((intmax_t)(signed char)va_arg(p->args, signed int));
	else if (p->flags & H_FLAG)
		return ((intmax_t)(signed short)va_arg(p->args, signed int));
	else if (p->flags & LL_FLAG)
		return ((intmax_t)va_arg(p->args, signed long long));
	else if (p->flags & J_FLAG)
		return (va_arg(p->args, intmax_t));
	else if (p->flags & Z_FLAG)
		return ((intmax_t)va_arg(p->args, ssize_t));
	else if (p->flags & T_FLAG)
		return ((intmax_t)va_arg(p->args, ptrdiff_t));
	else
		return ((intmax_t)va_arg(p->args, signed int));
}

/*
**	handle_int()
**	We have a signed int!
**	1) Get the va_arg and cast it as maxint_t.
**	2) Check if we have a negative int. We'll need to remember that!
**	3) Count the number of digits we have in our int.
**	4) If we have the "'" flag we need to calculate how many commas or periods
**	   need to be added to our nbr_len
**	5) If 'nbr == 0', then we need to make space for it.
**	   EXCEPT WHEN THE PRECISION IS 0!
**	5) If we have a negative number, a PLUS_FLAG, or SPACE_FLAG that takes up
**	   space so we'll need less width padding.
**	6) We handle the complex prepadding (see: printf_num_utils.c)
**	7) We run our itoa_base (printf version) and pass the absolute val
**	   of our number cast as an max unsigned int (that way we can reuse our
**	   itoa function for other stuff!) (see: printf_num_utils.c)
**	8) We add ' ' padding if have the DASH_FLAG (left justified)
*/

void			handle_int(t_vfpf *p)
{
	intmax_t	nbr;
	intmax_t	tmp;
	int			nbr_len;
	int			pf;

	pf = p->flags;
	p->base = 10;
	nbr = get_signed_int_arg(p);
	tmp = nbr;
	nbr_len = 0;
	p->neg = (nbr < 0) ? 1 : 0;
	while (tmp && ++nbr_len)
		tmp /= p->base;
	nbr_len += (pf & APOST_FLAG) ? ((nbr_len - 1) / 3) : 0;
	nbr_len += (nbr == 0 && !(pf & PRECI_OB_FLAG && p->precision == 0)) ? 1 : 0;
	if (nbr < 0 || pf & PLUS_FLAG || pf & SPACE_FLAG)
		p->width--;
	if (pf & PRECI_OB_FLAG)
		pf ^= ZERO_FLAG;
	handle_int_prepad(p, nbr_len, 1);
	pf_itoa_base(p, (uintmax_t)ABS(nbr), nbr_len);
	if (pf & WIDTH_OB_FLAG && (pf & DASH_FLAG))
		pad_width(p, MAX(p->precision, nbr_len));
}
