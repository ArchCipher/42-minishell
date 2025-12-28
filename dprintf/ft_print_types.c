/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_types.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 12:38:21 by kmurugan          #+#    #+#             */
/*   Updated: 2025/11/19 17:40:11 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_dprintf.h"

/*
DESCRIPTION
	Prints a single character with width formatting.
	Returns the number of characters printed or -1 on error.

	Values ignored: precision, dot, sign, hash, '0' pad
*/

int	ft_print_char(char c, t_fmt flag)
{
	int		space_pad;
	ssize_t	written;

	space_pad = 0;
	if (flag.align != '-')
		space_pad = print_pad(max(0, flag.width - 1), flag.pad, flag.fd);
	written = write(flag.fd, &c, 1);
	if (flag.align == '-')
		space_pad = print_pad(max(0, flag.width - 1), flag.pad, flag.fd);
	if (space_pad < 0 || written < 0)
		return (-1);
	return (space_pad + (int)written);
}

/*
DESCRIPTION
	Prints a string with width and precision formatting.
	NULL strings are replaced with "(null)".
	Returns the number of characters printed or -1 on error.

	Values ignored: sign, hash, '0' pad
*/

int	ft_print_str(char *s, t_fmt flag)
{
	ssize_t	written;
	int		pad_len;
	int		width_pad;
	size_t	len;

	if (!s)
		s = "(null)";
	len = ft_strlen(s);
	pad_len = 0;
	if (flag.dot && (size_t)flag.precision < len)
		len = flag.precision;
	if ((size_t)flag.width > len)
		pad_len = flag.width - len;
	if (len > INT_MAX || (size_t)pad_len > INT_MAX - len)
		return (-1);
	if (flag.align != '-')
		width_pad = print_pad(pad_len, flag.pad, flag.fd);
	written = write(flag.fd, s, len);
	if (flag.align == '-')
		width_pad = print_pad(pad_len, flag.pad, flag.fd);
	if (width_pad < 0 || written < 0 || written != (ssize_t)len
		|| (int)written > INT_MAX - width_pad)
		return (-1);
	return (width_pad + (int)written);
}
/*
DESCRIPTION
	Converts signed integer to unsigned and prints it.
	Handles negative sign prefix for negative numbers.
	Returns the number of characters printed or -1 on error.
*/

int	ft_print_int(int i, t_fmt *flag)
{
	if (i >= 0)
		return (ft_itoa_base((unsigned int)i, 10, *flag));
	flag->sign = '-';
	return (ft_itoa_base((unsigned int)(0u - (unsigned int)i), 10, *flag));
}

int	max(int a, int b)
{
	if (a > b)
		return (a);
	return (b);
}
