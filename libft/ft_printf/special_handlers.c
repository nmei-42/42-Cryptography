/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   special_handlers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/21 13:52:46 by nmei              #+#    #+#             */
/*   Updated: 2018/01/04 13:52:02 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>

void			handle_escape(t_vfpf *p)
{
	int		pf;

	pf = p->flags;
	if (pf & WIDTH_OB_FLAG && !(pf & DASH_FLAG))
		pad_width(p, 1);
	buff(p, "%", 1);
	if (pf & WIDTH_OB_FLAG && (pf & DASH_FLAG))
		pad_width(p, 1);
}

void			handle_n(t_vfpf *p)
{
	if (p->flags & HH_FLAG)
		*va_arg(p->args, signed char *) = (signed char)p->len;
	else if (p->flags & H_FLAG)
		*va_arg(p->args, short int *) = (short int)p->len;
	else if (p->flags & L_FLAG)
		*va_arg(p->args, long int *) = (long int)p->len;
	else if (p->flags & LL_FLAG)
		*va_arg(p->args, long long int *) = (long long int)p->len;
	else if (p->flags & J_FLAG)
		*va_arg(p->args, intmax_t *) = (intmax_t)p->len;
	else if (p->flags & Z_FLAG)
		*va_arg(p->args, ssize_t *) = (ssize_t)p->len;
	else if (p->flags & T_FLAG)
		*va_arg(p->args, ptrdiff_t *) = (ptrdiff_t)p->len;
	else
		*va_arg(p->args, int *) = (int)p->len;
}
