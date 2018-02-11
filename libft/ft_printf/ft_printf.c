/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/18 20:55:16 by nmei              #+#    #+#             */
/*   Updated: 2018/01/30 15:07:26 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <libft.h>
#include <ft_printf.h>

/*
**	Parse the format string we just encountered!
**	%[flags][width][.precision][length]specifier
**
**	We'll separate into separate parsing steps
**	[flags]: '-', '+', ' ', '#', '0'
**	[width]: 'number', '*'
**	[.precision]: '.number', '.*'
**	[length]: '(none)', 'hh', 'h', 'l' (ell), 'll', 'j', 'z', 't', 'L'
**	specifier: 'di', 'ouxX', 'fFeEgGaA', 'c', 's', 'p', 'n'
**
**	If in the process of parsing, we find to our horror that we have an invalid
**	format string, then we will PURPOSELY treat the format string as a normal
**	string and just print it out (and move on).
*/

static void	parse_format(t_vfpf *p)
{
	t_jumptable		handler_funct;
	const char		*fmt_start;

	fmt_start = p->fmt;
	if (p->fmt++ != '\0')
	{
		p->strt = p->fmt;
		parse_flags(p);
		parse_width(p);
		parse_precision(p, 0);
		parse_length(p);
		p->specifier = *p->fmt;
		if ((handler_funct = get_handler_funct(p->specifier)) == NULL)
			p->flags |= FMT_INVALID;
		else if (!(p->flags & FMT_INVALID))
		{
			handler_funct(p);
			p->fmt++;
		}
	}
	p->strt = (p->flags & FMT_INVALID) ? fmt_start : p->fmt;
}

/*
**	ft_vdprintf - The *real* printf.
**	We will use the t_vfpf struct to store everything from our format string
**	to copies of the variatic arguments. So definitely take a look at the
**	ft_printf.h file to get an idea of what's going on.
*/

int			ft_vdprintf(int fd, const char *format, va_list args)
{
	t_vfpf	p;

	ft_bzero(&p, sizeof(p));
	p.fd = fd;
	va_copy(p.args, args);
	p.fmt = format;
	p.strt = p.fmt;
	while (*p.fmt)
	{
		if (*p.fmt != '%')
			p.fmt++;
		else
		{
			buff(&p, p.strt, p.fmt - p.strt);
			parse_format(&p);
			reset_printf(&p);
		}
	}
	buff(&p, p.strt, p.fmt - p.strt);
	flush_buffer(&p);
	va_end(p.args);
	return (p.len);
}

/*
**	ft_printf
**	This will just be a wrapper around our real printf function ft_vdprintf!
*/

int			ft_printf(const char *format, ...)
{
	va_list	args;
	int		len;

	len = 0;
	if (format)
	{
		va_start(args, format);
		len = ft_vdprintf(1, format, args);
		va_end(args);
	}
	return (len);
}

/*
**	ft_dprintf
**	This will just be a wrapper around our real printf function ft_vdprintf!
*/

int			ft_dprintf(int fd, const char *format, ...)
{
	va_list	args;
	int		len;

	len = 0;
	if (format)
	{
		va_start(args, format);
		len = ft_vdprintf(fd, format, args);
		va_end(args);
	}
	return (len);
}
