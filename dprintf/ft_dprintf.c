#include "ft_dprintf.h"

static ssize_t	print_literal(int fd, const char **fmt);
static size_t	ft_strcspn_char(const char *s, int c);

/*
	NAME
		ft_dprintf
	SYNOPSIS
		#include "ft_dprintf.h"
	PROTOTYPE
		int ft_dprintf(int fd, const char *, ...);
	DESCRIPTION:
		Prints a formatted string to the file descriptor.
		Conversions are: cspdiuxX%
	RETURN VALUE
		Returns the number of characters printed.
		The printf utility exits 0 on success, and >0 if an error occurs.
	EXTERNAL FUNC(S)
		write, va_start, va_arg, va_end
*/

int ft_dprintf(int fd, const char *fmt, ...)
{
	int		ret;
	va_list	ap;

	va_start(ap, fmt);
	ret = ft_vdprintf(fd, fmt, ap);
	va_end(ap);
	return (ret); 
}

int	ft_vdprintf(int fd, const char *fmt, va_list ap)
{
	int		ret;
	ssize_t	written;
	t_fmt	flag;

	ret = 0;
	while (*fmt)
	{
		if (*fmt == '%')
		{
			fmt++;
			if (!parse_fmt(&fmt, &flag))
				return (-1);
			flag.fd = fd;
			written = handle_spec(ap, &flag);
		}
		else
			written = print_literal(fd,&fmt);
		if (written < 0 || written > INT_MAX || (int)written > (INT_MAX - ret))
			return (-1);
		ret += written;
	}
	return (ret);
}

static ssize_t	print_literal(int fd, const char **fmt)
{
	ssize_t	written;
	size_t	len;

	len = ft_strcspn_char(*fmt, '%');
	if (len > INT_MAX)
		return (-1);
	written = write(fd, *fmt, len);
	if (written < 0)
		return (-1);
	*fmt += len;
	return (written);
}

/*
DESCRIPTION:
	Returns the number of bytes in the initial part of 's' which precedes
	the first occurrence of character 'c'. If 'c' is not found, returns
	the length of the string.

	Simplified version of strcspn(const char *s, const char *charset);
*/

static size_t	ft_strcspn_char(const char *s, int c)
{
	size_t	n;

	n = 0;
	while (s[n] && s[n] != (unsigned char)c)
		n++;
	return (n);
}
