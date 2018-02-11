/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printf_utf_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/21 16:45:26 by nmei              #+#    #+#             */
/*   Updated: 2018/01/04 13:52:09 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <wchar.h>
#include <libft.h>
#include <ft_printf.h>

/*
**	pf_wcharlen()
**	Get the length of our wide char
**
**	Useful resource: https://en.wikipedia.org/wiki/UTF-8
**
**	Largest a single width wide char can be 0x007F
**	Largest a double width wide char can be 0x07FF
**	Largest a triple width wide char can be 0xFFFF
**	Largest a quad width wide char can be 0x10FFFF
*/

size_t	pf_wcharlen(wchar_t wc)
{
	if (wc <= 0x007F)
		return (1);
	else if (wc <= 0x07FF)
		return (2);
	else if (wc <= 0xFFFF)
		return (3);
	else if (wc <= 0x10FFFF)
		return (4);
	return (0);
}

/*
**	pf_wstrlen()
**	very similar to ft_strlen except that because wide chars can be variable
**	size we can't just do wlen++ and must instead accumulate the sum of
**	each wide character in the wide string.
*/

size_t	pf_wstrlen(wchar_t *wstr)
{
	size_t	wlen;

	wlen = 0;
	while (*wstr != L'\0')
	{
		wlen += pf_wcharlen(*wstr);
		wstr++;
	}
	return (wlen);
}

void	pf_putchar(t_vfpf *p, char c)
{
	buff(p, &c, 1);
}

/*
**	write_wchar()
**
**	Because it's worth repeating:
**	Useful resource: https://en.wikipedia.org/wiki/UTF-8
**
**	For 1 byte UTF the mask looks like 0xxxxxxx
**	For 2 byte UTF the mask looks like 110xxxxx for 1st byte then 10xxxxxx
**	for all subsequent bytes
**	For 3 byte UTF the mask looks like 1110xxxx for 1st byte then 10xxxxxx
**	for all subsequent bytes
**	For 4 byte UTF the mask looks like 11110xxx for 1st byte then 10xxxxxx
**	for all subsequent bytes
**
**	For stuff <= 0x7F (01111111) we can just directly write it out.
**
**	For stuff <= 0x07FF (11111111111) we bit shift right by 6 to get the left-
**	most 5 1's (11111) and we 'OR' it with (110xxxxx) == (0xC0). We then take
**	the remaining 6 1's (111111) using (0x3F) == (111111) as a mask and
**	'OR' it with (0x80) == (10000000).
**
**	We continue in a similar manner but use the appropriate mask for the first
**	byte (i.e. 0, 0xC0, 0xE0, 0xF0).
*/

void	pf_putwchar(t_vfpf *p, wint_t wc)
{
	if (wc <= 0x007F)
		pf_putchar(p, wc);
	else if (wc <= 0x07FF)
	{
		pf_putchar(p, ((wc >> 6) | 0xC0));
		pf_putchar(p, ((wc & 0x3F) | 0x80));
	}
	else if (wc <= 0xFFFF)
	{
		pf_putchar(p, ((wc >> 12) | 0xE0));
		pf_putchar(p, (((wc >> 6) & 0x3F) | 0x80));
		pf_putchar(p, ((wc & 0x3F) | 0x80));
	}
	else if (wc <= 0x10FFFF)
	{
		pf_putchar(p, ((wc >> 18) | 0xF0));
		pf_putchar(p, (((wc >> 12) & 0x3F) | 0x80));
		pf_putchar(p, (((wc >> 6) & 0x3F) | 0x80));
		pf_putchar(p, ((wc & 0x3F) | 0x80));
	}
}

/*
**	pf_putwstr()
**
**	Function to write out only 'wlen' wide chars of a wide string.
**	We use i to precalculate the width of the current wide char in the wstr that
**	we haven't written out yet.
**
**	If i is <= wlen we can write out the current wide char.
**	NOTE: It is possible to have a situation where the number of characters
**		  does not match wlen. This will need to be accounted for...
**	example: 'i = 5' 'wlen = 7' '*wstr == 0x10FFFF'
*/

void	pf_putwstr(t_vfpf *p, wchar_t *wstr, size_t wlen)
{
	size_t	i;

	i = 0;
	while (*wstr && i < wlen)
	{
		if (*wstr <= 0x7F)
			i++;
		else if (*wstr <= 0x7FF)
			i += 2;
		else if (*wstr <= 0xFFFF)
			i += 3;
		else if (*wstr <= 0x10FFFF)
			i += 4;
		if (i <= wlen)
			pf_putwchar(p, *wstr++);
	}
}
