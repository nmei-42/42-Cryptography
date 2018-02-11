/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/19 13:15:33 by nmei              #+#    #+#             */
/*   Updated: 2018/01/30 15:05:31 by nmei             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H
# include <stddef.h>
# include <sys/types.h>
# include <stdint.h>
# include <stdarg.h>
# include <wchar.h>
# include <unistd.h>

# define HASH_FLAG		1
# define ZERO_FLAG		2
# define DASH_FLAG		4
# define PLUS_FLAG		8
# define SPACE_FLAG		16
# define APOST_FLAG		32
# define LDTOA_FRAC		64
# define H_FLAG			128
# define HH_FLAG		256
# define L_FLAG			512
# define LL_FLAG		1024
# define J_FLAG			2048
# define Z_FLAG			4096
# define T_FLAG 		8192
# define FLOAT_L_FLAG	16384
# define WIDTH_OB_FLAG	32768
# define PRECI_OB_FLAG	65536
# define LEN_OB_FLAG	131072
# define FMT_INVALID	262144

/*
**	Structure containing all useful variables for our printf functions!
**
**	flags uses a binary addressing system to indicate various things about
**	our printf format string (including handling status!!)
**
**	[flags]
**	'#'						: 1  (force prefix)
**	'0'						: 2  (left pad)
**	'-'						: 4  (left justify)
**	'+'						: 8  (show sign)
**	' '						: 16 (insert space)
**	'''						: 32 (format integers with locale's 1000's char)
**
**							: 64 (reserved for possible future uses)
**
**	'h'						: 128
**	'hh'					: 256
**	'l'						: 512
**	'll'					: 1024
**	'j'						: 2048
**	'z'						: 4096
**	't'						: 8192
**	'L'						: 16384
**
**	'width obtained'		: 32768
**	'precision obtained'	: 65536
**	'length obtained'		: 131072
**
**	'formatter invalid'		: 262144
*/

# define MAX(x, y) (((x) > (y)) ? (x) : (y))
# define ABS(x) (((x) < 0) ? -(x) : (x))

/*
**	Color definitions
**
**	Useful resource:
**	http://tldp.org/HOWTO/Bash-Prompt-HOWTO/x329.html
**
**	To printf a green 'foo':
**	echo "\e[00;32mfoo\e[m"
**
**	Note: \e is a shortcut for 'escape' (ascii char 27)
**	(also might be represented as octal \033 or hex \0x1b)
**
**	Thus these are all valid ways of getting a green 'foo'
**	echo "\033[00;32mfoo\033[m"
**	echo "\0x1b[00;32mfoo\0x1b[m"
**
**	Doesn't seem worthwhile to implement in ft_printf because you can just do:
**	printf("\033[00;32mfoo\033[m");
**
**	If you're feeling tricky then just make the below definitions into a color.h
**	#include "color.h" the file then you can just do:
**	printf("%sfoo%s", GREEN, EOC);
*/

# define BLACK		"\e[00;30m"
# define DARK_GRAY	"\e[01;30m"
# define RED		"\e[00;31m"
# define L_RED		"\e[01;31m"
# define GREEN		"\e[00;32m"
# define L_GREEN	"\e[01;32m"
# define BROWN		"\e[00;33m"
# define YELLOW		"\e[01;33m"
# define BLUE		"\e[00;34m"
# define L_BLUE		"\e[01;34m"
# define PURPLE		"\e[00;35m"
# define L_PURPLE	"\e[01;35m"
# define CYAN		"\e[00;36m"
# define L_CYAN		"\e[01;36m"
# define L_GRAY		"\e[00;37m"
# define WHITE		"\e[01;37m"
# define EOC		"\e[m"

# define BUFF_SIZE	4096
# define ULL		unsigned long long

typedef struct	s_vfpf
{
	const char	*fmt;
	const char	*strt;
	va_list		args;
	int			fd;
	size_t		len;
	size_t		buf_len;
	char		buf[BUFF_SIZE];
	int			flags;
	int			width;
	int			precision;
	int			base;
	int			neg;
	uintmax_t	hex_int;
	char		specifier;
}				t_vfpf;

union			u_union
{
	ULL			l;
	double		d;
}				u;

/*
**	ft_printf.c
**	Main printf functions!
**	Why have a vfprintf? See: https://stackoverflow.com/questions/1485805/
*/
int				ft_vdprintf(int fd, const char *format, va_list args);
int				ft_printf(const char *format, ...);
int				ft_dprintf(int fd, const char *format, ...);

/*
**	printf_gen_utils.c
*/
void			flush_buffer(t_vfpf *p);
void			buff(t_vfpf *p, const void *s, size_t nbyte);
void			pad(t_vfpf *p, int pad_len, char pad_char);
void			pad_width(t_vfpf *p, int arg_width);
void			reset_printf(t_vfpf *p);

/*
**	printf_utf_utils.c
*/
size_t			pf_wcharlen(wchar_t wc);
size_t			pf_wstrlen(wchar_t *wstr);
void			pf_putchar(t_vfpf *p, char c);
void			pf_putwchar(t_vfpf *p, wint_t wc);
void			pf_putwstr(t_vfpf *p, wchar_t *wstr, size_t wlen);

/*
**	printf_int_utils.c
*/
void			pf_itoa_base(t_vfpf *p, uintmax_t nbr, int nbr_len);
void			handle_int_prepad(t_vfpf *p, int nbr_len, int signed_int);

/*
**	printf_double_utils.c
*/
void			pf_ldtoa_base(t_vfpf *p, long double nbr, size_t int_len,
							size_t tot_len);
void			handle_double_prepad(t_vfpf *p, int nbr_len);
/*
**	parse_prespecifiers.c
**
**	We'll separate into separate parsing steps:
**	[flags]: '-', '+', ' ', '#', '0'
**	[width]: 'number', '*'
**	[.precision]: '.number', '.*'
**	[length]: '(none)', 'hh', 'h', 'l', 'll', 'j', 'z', 't', 'L'
*/
void			parse_flags(t_vfpf *p);
void			parse_width(t_vfpf *p);
void			parse_precision(t_vfpf *p, int recurse_level);
void			parse_length(t_vfpf *p);

/*
**	parse_specifier.c
**
**	After parsing pre-specifiers we'll use a jump table to get proper
**	handlers for all the different possible specifiers.
**	specifiers: '%', 'dDi', 'oOuUxX', 'fFeEgGaA', 'cC', 'sS', 'p', 'n'
*/
typedef void	(*t_jumptable)(t_vfpf *p);
t_jumptable		get_handler_funct(char c);

/*
**	int_handlers.c
**	handles 'dD', and 'i' specifiers
*/
void			handle_int(t_vfpf *p);

/*
**	uint_handlers.c
**	handles 'oO', 'uU', 'xX', and 'p' specifiers
*/
void			handle_unsigned_int_oct(t_vfpf *p);
void			handle_unsigned_int_dec(t_vfpf *p);
void			handle_unsigned_int_hex(t_vfpf *p);

/*
**	double_handlers.c
*/
long double		get_ld_arg(t_vfpf *p, size_t *len, int base);
void			handle_double(t_vfpf *p);

/*
**	hard_double_handlers.c - Sorry not implemented currently...
**	Maybe in the future!!!
*/
void			handle_double_sci(t_vfpf *p);
void			handle_double_shortest(t_vfpf *p);
void			handle_double_hex(t_vfpf *p);

/*
**	str_char_handlers.c
**	handles 'cC' and 'sS' specifiers
*/
void			handle_char(t_vfpf *p);
void			handle_str(t_vfpf *p);

/*
**	special_handlers.c
**	handles '%' and 'n' specifiers.
*/
void			handle_escape(t_vfpf *p);
void			handle_n(t_vfpf *p);

#endif
