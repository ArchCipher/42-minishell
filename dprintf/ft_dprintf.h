/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 12:27:51 by kmurugan          #+#    #+#             */
/*   Updated: 2025/11/19 17:40:25 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_DPRINTF_H
# define FT_DPRINTF_H

# include <limits.h> // INT_MAX
# include <stdarg.h> // va_start, va_arg, va_end
# include <unistd.h> // write

# define FMT_SPEC "cspdiuxX%"
# define FLAGS "-+ 0#"
# define DECIMAL "0123456789"
# define LOWER_HEX "0123456789abcdef"
# define UPPER_HEX "0123456789ABCDEF"
# define OCTAL "01234567"
# define BUF_SIZE 20

# ifdef __linux__
#  define NULL_PTR_STR "(nil)"
# else
#  define NULL_PTR_STR "0x0"
# endif

typedef struct s_fmt
{
	int		fd;
	char	align;
	char	pad;
	char	sign;
	char	hash;
	int		width;
	char	dot;
	int		precision;
	char	spec;
}			t_fmt;

// ft_dprintf
int			ft_dprintf(int fd, const char *fmt, ...);
int			ft_vdprintf(int fd, const char *fmt, va_list ap);

// parser
int			parse_fmt(const char **fmt, t_fmt *flag);
int			handle_spec(va_list ap, t_fmt *flag);

// print types
int			ft_print_char(char c, t_fmt flag);
int			ft_print_str(char *s, t_fmt flag);
int			ft_print_int(int i, t_fmt *flag);
int			max(int a, int b);

// print int
int			ft_itoa_base(unsigned long long n, int base, t_fmt flag);
int			print_pad(int pad_len, char pad, int fd);

// printf_utils
int			ft_isdigit(int c);
int			ft_atoi_positive(const char *str);
size_t		ft_strlen(const char *s);
void		*ft_memset(void *b, int c, size_t len);
char		*ft_strchr(const char *s, int c);

#endif
