/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uint_handlers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/02 12:47:32 by nmei              #+#    #+#             */
/*   Updated: 2018/01/04 20:54:35 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <ft_printf.h>
#define UPPER(x) ((x)=='X'||(x)=='F'||(x)=='E'||(x)=='G'||(x)=='A')

/*
**	get_unsigned_int_arg()
**	If we're handling uints then we need to invoke va_arg. Because dealing
**	with different size possibilities is a pain, it's easiest to promote
**	everything to the max uint size type.
**
**	We have to do a slight shuffle so that L_FLAG is checked before
**	H_FLAG because U and O specifier should take precedence...
**
**	pointer addresses ('p') are simply a special case of hex uints.
*/

static	uintmax_t	get_unsigned_int_arg(t_vfpf *p)
{
	if (p->specifier == 'p')
		return ((uintmax_t)(uintptr_t)va_arg(p->args, void *));
	else if (p->flags & L_FLAG || p->specifier == 'O' || p->specifier == 'U')
		return ((uintmax_t)va_arg(p->args, unsigned long int));
	else if (p->flags & HH_FLAG)
		return ((uintmax_t)(unsigned char)va_arg(p->args, unsigned int));
	else if (p->flags & H_FLAG)
		return ((uintmax_t)(unsigned short int)va_arg(p->args, unsigned int));
	else if (p->flags & LL_FLAG)
		return ((uintmax_t)va_arg(p->args, unsigned long long int));
	else if (p->flags & J_FLAG)
		return (va_arg(p->args, uintmax_t));
	else if (p->flags & Z_FLAG)
		return ((uintmax_t)va_arg(p->args, ssize_t));
	else if (p->flags & T_FLAG)
		return ((uintmax_t)va_arg(p->args, ptrdiff_t));
	else
		return ((uintmax_t)va_arg(p->args, unsigned int));
}

void				handle_unsigned_int_dec(t_vfpf *p)
{
	uintmax_t	nbr;
	uintmax_t	tmp;
	int			nbr_len;
	int			pf;

	pf = p->flags;
	p->base = 10;
	nbr = get_unsigned_int_arg(p);
	tmp = nbr;
	nbr_len = 0;
	while (tmp && ++nbr_len)
		tmp /= p->base;
	nbr_len += (nbr == 0 && !(pf & PRECI_OB_FLAG && p->precision == 0)) ? 1 : 0;
	nbr_len += (pf & APOST_FLAG) ? ((nbr_len - 1) / 3) : 0;
	if (pf & PRECI_OB_FLAG)
		pf ^= ZERO_FLAG;
	handle_int_prepad(p, nbr_len, 0);
	pf_itoa_base(p, (uintmax_t)ABS(nbr), nbr_len);
	if (pf & WIDTH_OB_FLAG && (pf & DASH_FLAG))
		pad_width(p, MAX(p->precision, nbr_len));
}

void				handle_unsigned_int_oct(t_vfpf *p)
{
	uintmax_t	nbr;
	uintmax_t	tmp;
	int			nbr_len;
	int			pf;

	pf = p->flags;
	p->base = 8;
	nbr = get_unsigned_int_arg(p);
	tmp = nbr;
	nbr_len = 0;
	while (tmp && ++nbr_len)
		tmp /= p->base;
	nbr_len += (nbr == 0 && !(pf & PRECI_OB_FLAG && p->precision == 0)) ? 1 : 0;
	if (pf & PRECI_OB_FLAG)
		pf ^= ZERO_FLAG;
	if (pf & HASH_FLAG && nbr != 0)
		p->width--;
	handle_int_prepad(p, nbr_len, 0);
	if ((pf & HASH_FLAG && nbr != 0) || (pf & HASH_FLAG && pf & PRECI_OB_FLAG))
		buff(p, "0", 1);
	pf_itoa_base(p, (uintmax_t)ABS(nbr), nbr_len);
	if (pf & WIDTH_OB_FLAG && (pf & DASH_FLAG))
		pad_width(p, MAX(p->precision, nbr_len));
}

void				handle_unsigned_int_hex(t_vfpf *p)
{
	uintmax_t	nbr;
	uintmax_t	tmp;
	int			nbr_len;
	int			pf;

	if (p->specifier == 'p')
		p->flags |= HASH_FLAG;
	pf = p->flags;
	p->base = 16;
	nbr = get_unsigned_int_arg(p);
	p->hex_int = nbr;
	tmp = nbr;
	nbr_len = 0;
	while (tmp && ++nbr_len)
		tmp /= p->base;
	nbr_len += (nbr == 0 && !(pf & PRECI_OB_FLAG && p->precision == 0)) ? 1 : 0;
	if (pf & PRECI_OB_FLAG)
		pf ^= ZERO_FLAG;
	if (pf & HASH_FLAG && (nbr != 0 || p->specifier == 'p'))
		p->width -= 2;
	handle_int_prepad(p, nbr_len, 0);
	pf_itoa_base(p, (uintmax_t)ABS(nbr), nbr_len);
	if (pf & WIDTH_OB_FLAG && (pf & DASH_FLAG))
		pad_width(p, MAX(p->precision, nbr_len));
}
