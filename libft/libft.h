/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 18:01:41 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 14:06:00 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <limits.h> // LONG_MAX
# include <stdint.h> // SIZE_MAX
# include <stdlib.h> // malloc
# include <unistd.h> // write

// libc functions
int		ft_isalnum(int c);
int		ft_isalpha(int c);
int		ft_isdigit(int c);
int		ft_isspace(int c);
void	*ft_memchr(const void *s, int c, size_t n);
void	*ft_memcpy(void *dst, const void *src, size_t n);
void	*ft_memset(void *b, int c, size_t len);
void	*ft_realloc(void *ptr, size_t old_size, size_t size);
char	*ft_strchr(const char *s, int c);
int		ft_strcmp(const char *s1, const char *s2);
size_t	ft_strcspn(const char *s, const char *reject);
char	*ft_strdup(const char *s1);
size_t	ft_strlen(const char *s);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_strndup(const char *s1, size_t len);
size_t	ft_strspn(const char *s, const char *accept);
char	*ft_strtok_r(char *s, const char *sep, char **p);

// additional functions
char	*ft_strjoin(const char *s1, const char *s2);
char	*ft_strjoin3(const char *s1, const char *s2, const char *s3);
int		ft_putstr(const char *s, int fd);
#endif
