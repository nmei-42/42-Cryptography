/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printf_gen_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/21 16:45:26 by nmei              #+#    #+#             */
/*   Updated: 2018/01/05 16:21:01 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <libft.h>
#include <ft_printf.h>

void	flush_buffer(t_vfpf *p)
{
	write(p->fd, p->buf, p->buf_len);
	p->buf_len = 0;
}

/*
**	buff()
**	We will buffer our printf to avoid the overhead of multiple 'write(2)' calls
**	Written to closely mirror the arguments of 'write(2)' so we can just do a
**	simple replace.
**
**	1) cast our void *s as an unsigned char
**	2) While our current buf_len and the nbytes we'd like to write are bigger
**	   than our BUFF_SIZE
**	   a) We calculate the remaining space in our buffer
**	   b) We fill the buffer to the brim (and decrement nbyte for each fill)
**	   c) We flush the buffer
**  3) Buffer any left over nbytes now that we know that 'p->buf_len + nbyte'
**	   will fit in our buffer.
*/

void	buff(t_vfpf *p, const void *s, size_t nbyte)
{
	unsigned char	*src;
	size_t			remain;

	src = (unsigned char *)s;
	while (p->buf_len + nbyte > BUFF_SIZE)
	{
		remain = BUFF_SIZE - p->buf_len;
		while (remain-- && nbyte--)
		{
			p->buf[p->buf_len++] = *src++;
			p->len++;
		}
		flush_buffer(p);
	}
	while (nbyte--)
	{
		p->buf[p->buf_len++] = *src++;
		p->len++;
	}
}

/*
**	pad()
**	General function that generates a character string of pad_len consisting
**	of the 'pad_char's which then gets sent to the buffer.
*/

void	pad(t_vfpf *p, int pad_len, char pad_char)
{
	char	*pad_str;

	pad_str = NULL;
	if (pad_len > 0)
	{
		if ((pad_str = (char *)malloc(pad_len + 1)) != NULL)
		{
			pad_str[pad_len] = '\0';
			ft_memset(pad_str, pad_char, (size_t)pad_len);
			buff(p, pad_str, pad_len);
			free(pad_str);
		}
	}
}

/*
**	pad_width()
**	Function to calculate padding that should be added for given specified
**	width and for given argument width. Strictly only pads spaces thus specific
**	for the width prespecifier.
*/

void	pad_width(t_vfpf *p, int arg_width)
{
	int final_pad_width;

	final_pad_width = p->width - arg_width;
	pad(p, final_pad_width, ' ');
}

void	reset_printf(t_vfpf *p)
{
	p->flags = 0;
	p->width = 0;
	p->precision = 0;
	p->base = 0;
	p->neg = 0;
	p->hex_int = 0;
	p->specifier = '\0';
}
