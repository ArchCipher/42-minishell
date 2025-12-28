/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 18:01:41 by kmurugan          #+#    #+#             */
/*   Updated: 2025/10/17 20:23:11 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <limits.h> // LONG_MAX
# include <stdint.h> // SIZE_MAX
# include <stdlib.h> // malloc
# include <unistd.h> // write

// typedef struct s_list
// {
// 	void			*content;
// 	struct s_list	*next;
// }					t_list;

// libc functions
int					ft_isalpha(int c);
int					ft_isdigit(int c);
int					ft_isalnum(int c);
// int					ft_isascii(int c);
// int					ft_isprint(int c);
// int					ft_toupper(int c);
// int					ft_tolower(int c);
size_t				ft_strlen(const char *s);
// size_t				ft_strlcpy(char *dst, const char *src, size_t dstsize);
// size_t				ft_strlcat(char *dst, const char *src, size_t dstsize);
char				*ft_strchr(const char *s, int c);
char				*ft_strrchr(const char *s, int c);
int					ft_strncmp(const char *s1, const char *s2, size_t n);
// char				*ft_strnstr(const char *haystack, const char *needle,
// 						size_t len);
// void				*ft_memset(void *b, int c, size_t len);
// void				ft_bzero(void *s, size_t n);
void				*ft_memcpy(void *dst, const void *src, size_t n);
// void				*ft_memccpy(void *dst, const void *src, int c, size_t n);
// void				*ft_memmove(void *dst, const void *src, size_t len);
void				*ft_memchr(const void *s, int c, size_t n);
// int					ft_memcmp(const void *s1, const void *s2, size_t n);
// int					ft_atoi(const char *str);
// void				*ft_calloc(size_t count, size_t size);
char				*ft_strdup(const char *s1);

// additional functions
// char				*ft_substr(const char *s, unsigned int start, size_t len);
char				*ft_strjoin(const char *s1, const char *s2);
// char				*ft_strtrim(const char *s1, const char *set);
// char				**ft_split(const char *s, char c);
// char				*ft_itoa(int n);
// char				*ft_strmapi(const char *s, char (*f)(unsigned int, char));
// void				ft_striteri(char *s, void (*f)(unsigned int, char *));
// void				ft_putchar_fd(char c, int fd);
// void				ft_putstr_fd(char *s, int fd);
// void				ft_putendl_fd(char *s, int fd);
// void				ft_putnbr_fd(int n, int fd);

/*
// bonus
t_list				*ft_lstnew(void *content);
void				ft_lstadd_front(t_list **lst, t_list *new);
int					ft_lstsize(t_list *lst);
t_list				*ft_lstlast(t_list *lst);
void				ft_lstadd_back(t_list **lst, t_list *new);
void				ft_lstdelone(t_list *lst, void (*del)(void *));
void				ft_lstclear(t_list **lst, void (*del)(void *));
void				ft_lstiter(t_list *lst, void (*f)(void *));
t_list				*ft_lstmap(t_list *lst, void *(*f)(void *),
						void (*del)(void *));
*/

// extra helpers
int					ft_isspace(int c);

// added later
int					ft_strcmp(const char *s1, const char *s2);
char				*ft_strndup(const char *s1, size_t len);
void				*ft_realloc(void *ptr, size_t old_size, size_t size);
char				*ft_strtok_r(char *s, const char *sep, char **p);
char	*ft_strjoin3(const char *s1, const char *s2, const char *s3);
#endif
