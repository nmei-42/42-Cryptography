/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_prespecifiers.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/19 22:45:41 by nmei              #+#    #+#             */
/*   Updated: 2018/01/03 15:57:43 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>
#include <ft_printf.h>
#define IS_FLAG(x) ((x)=='#'||(x)=='0'||(x)=='-'||(x)=='+'||(x)==' '||(x)=='\'')
#define IS_LEN(y) ((y)=='h'||(y)=='l'||(y)=='j'||(y)=='z'||(y)=='t'||(y)=='L')
#define IS_SLEN(z) ((z)=='j'||(z)=='z'||(z)=='t'||(z)=='L')

/*
**	Parse the format string we just encountered!
**	%[flags][width][.precision][length]specifier
**
**	We'll separate into separate parsing steps:
**	[flags]: '-', '+', ' ', '#', '0'
**	[width]: 'number', '*'
**	[.precision]: '.number', '.*'
**	[length]: '(none)', 'hh', 'h', 'l', 'll', 'j', 'z', 't', 'L'
**	specifier: 'di', 'ouxX', 'fFeEgGaA', 'c', 's', 'p', 'n'
*/

/*
**	parse_flags() - We'll use a binary numeral system similar to file
**	permissions to encode which flags we have in our format prespecifier.
**	This way we can encode all our flags in 1 variable and keep things clean.
**
**	Call the function recursively until we do not encounter any more flags...
**
**	'#' : 1		HASH_FLAG		(force prefix)
**	'0' : 2		ZERO_FLAG		(left pad)
**	'-' : 4		DASH_FLAG 		(left justify)
**	'+' : 8		PLUS_FLAG		(show sign)
**	' ' : 16	SPACE_FLAG		(insert space)
**	''' : 32	APOST_FLAG		(format ints with locale's 1000's char)
**
**	#0- = 1 + 2 + 4 = 7
**	- + = 4 + 16 + 8 = 28
**	etc...
**
**	Using bitwise 'or' we can easily set each flag:
**	EX: p->flag |= HASH_FLAG; set '#' flag to true
**
**	Using bitwise 'and' we can easily read out the status of each flag:
**	EX: 'if (p->flags & SPACE_FLAG)' checks IFF the ' ' flag is true
**	EX2: 'if (p->flags & DASH_FLAG)' checks IFF the '-' flag is true
*/

void			parse_flags(t_vfpf *p)
{
	if (IS_FLAG(*p->fmt))
	{
		if (*p->fmt == '#')
			p->flags |= HASH_FLAG;
		else if (*p->fmt == '0')
			p->flags |= ZERO_FLAG;
		else if (*p->fmt == '-')
			p->flags |= DASH_FLAG;
		else if (*p->fmt == '+')
			p->flags |= PLUS_FLAG;
		else if (*p->fmt == ' ')
			p->flags |= SPACE_FLAG;
		else if (*p->fmt == '\'')
			p->flags |= APOST_FLAG;
		p->fmt++;
		parse_flags(p);
	}
}

/*
**	parse_width()
**	For width we will either encounter either 'digits' or a 'wildcard'...
**	If we encounter a wildcard we'll read an argument out. Negative values for
**	the wildcard arg are invalid so we set our width = 0 if we get one...
**
**	We also want to check if someone tries to give a format like:
**	"%*5d" or "%5*d"
**
**	If we parse a '*'' or 'series of digits' then we set a width obtained flag.
**	If we found that this flag was already previously set, then we also need to
**	set a flag that our entire prespecifier is invalid.
**
**	We need to recursively call parse_width() because we might get an invalid
**	set of prespecifiers. Such as: "%*9*1*4d" (whyyyy would someone do this?)
*/

void			parse_width(t_vfpf *p)
{
	int tentative_width;

	if (!(p->flags & FMT_INVALID) && (*p->fmt == '*' || ft_isdigit(*p->fmt)))
	{
		p->flags |= (p->flags & WIDTH_OB_FLAG) ? FMT_INVALID : 0;
		if (!(p->flags & FMT_INVALID) && *p->fmt == '*')
		{
			p->fmt++;
			tentative_width = va_arg(p->args, int);
			p->flags |= (tentative_width < 0) ? DASH_FLAG : 0;
			p->width = ABS(tentative_width);
			p->flags |= WIDTH_OB_FLAG;
		}
		if (!(p->flags & FMT_INVALID) && ft_isdigit(*p->fmt))
		{
			p->flags |= (p->flags & WIDTH_OB_FLAG) ? FMT_INVALID : 0;
			while (!(p->flags & FMT_INVALID) && ft_isdigit(*p->fmt))
				p->width = p->width * 10 + (*(p->fmt++) - '0');
			p->flags |= WIDTH_OB_FLAG;
		}
		parse_width(p);
	}
}

/*
**	parse_precision()
**	For precision we will encounter a '.' followed by 'digits' or a 'wildcard'.
**	Very similar to parse_width. The only difference is the '.' and that we
**	store the precision in the p->precision variable.
**
**	This function will stall if the format string has multiple '.'s. We can use
**	this fact to check if the character in the parse_length() function starts
**	with a '.' to determine if we have an invalid prespecifier.
*/

void			parse_precision(t_vfpf *p, int recurse_level)
{
	if (!(p->flags & FMT_INVALID) && (*p->fmt == '.' || recurse_level > 0))
	{
		p->fmt = (*p->fmt == '.') ? p->fmt + 1 : p->fmt;
		if (*p->fmt == '*' || ft_isdigit(*p->fmt))
		{
			p->flags |= (p->flags & PRECI_OB_FLAG) ? FMT_INVALID : 0;
			if (!(p->flags & FMT_INVALID) && *p->fmt == '*')
			{
				p->fmt++;
				p->precision = va_arg(p->args, int);
				p->flags |= PRECI_OB_FLAG;
			}
			if (!(p->flags & FMT_INVALID) && ft_isdigit(*p->fmt))
			{
				p->flags |= (p->flags & PRECI_OB_FLAG) ? FMT_INVALID : 0;
				while (!(p->flags & FMT_INVALID) && ft_isdigit(*p->fmt))
					p->precision = p->precision * 10 + (*(p->fmt++) - '0');
				p->flags |= PRECI_OB_FLAG;
			}
			parse_precision(p, recurse_level + 1);
		}
		p->flags |= PRECI_OB_FLAG;
	}
}

/*
**	parse_long_length()
**	This function will specially parse our (potentially) long length options:
**	h, hh, l, and ll
**
**	If our format string equals the char we care about
**		If we have already obtained a length previously
**			If we have the single flag AND don't have the double flag
**				Use bitwise xor to set the single flag to 0
**				Use bitwise or to set our double flag to 1
**			else
**				We have an invalid format on our hands...
**		else
**			Use bitwise or to set our single flag to 1
**			Set our length obtained flag to 1
*/

static void		parse_long_length(t_vfpf *p, char c, int s_flag, int d_flag)
{
	if (*p->fmt == c)
	{
		if (p->flags & LEN_OB_FLAG)
		{
			if ((p->flags & s_flag) && !(p->flags & d_flag))
			{
				p->flags ^= s_flag;
				p->flags |= d_flag;
			}
			else
				p->flags |= FMT_INVALID;
		}
		else
		{
			p->flags |= s_flag;
			p->flags |= LEN_OB_FLAG;
		}
	}
}

/*
**	parse_length()
**	This function will parse all our length options. Need to call recursively
**	to check against crazy options like "hhh" or "jzjz" or "hj" or "hhL".
**
**	As mentioned in parse_precision() we also check for '.' in the case
**	that the format string has multiple '.'s
*/

void			parse_length(t_vfpf *p)
{
	if (!(p->flags & FMT_INVALID) && IS_LEN(*p->fmt))
	{
		parse_long_length(p, 'h', H_FLAG, HH_FLAG);
		parse_long_length(p, 'l', L_FLAG, LL_FLAG);
		if (IS_SLEN(*p->fmt))
		{
			if (!(p->flags & LEN_OB_FLAG))
			{
				p->flags |= (*p->fmt == 'j') ? J_FLAG : 0;
				p->flags |= (*p->fmt == 'z') ? Z_FLAG : 0;
				p->flags |= (*p->fmt == 't') ? T_FLAG : 0;
				p->flags |= (*p->fmt == 'L') ? FLOAT_L_FLAG : 0;
				p->flags |= LEN_OB_FLAG;
			}
			else
				p->flags |= FMT_INVALID;
		}
		p->fmt++;
		parse_length(p);
	}
	else if (*p->fmt == '.')
		p->flags |= FMT_INVALID;
}
