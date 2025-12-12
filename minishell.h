/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:33:15 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/10 20:39:27 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <errno.h>             // errno
# include <readline/history.h>  // readline (linux)
# include <readline/readline.h> // readline
# include <stdbool.h>           // boolean
# include <stdio.h>             // printf, readline     //, strerror, perror
# include <stdlib.h>            // free      // malloc, free, exit, getenv
# include <unistd.h>            // chdir,

# define BUFLEN 256
# define REJECT " \f\n\r\t\v<>|"
# define OPERATORS "<>|"

enum					e_token_type
{
	word,
	squote,
	dquote,
	pipe_char,
	redir_in,  // <
	redir_out, // >
	append,    // >>
	heredoc    // <<
				// assignment
};

typedef struct s_node
{
	enum e_token_type	type;
	char				*token;
	size_t				len;
	struct s_node		*next;
}						t_node;

typedef struct s_string
{
	char	*str;
	size_t	i;
	size_t	cap;
}			t_string;

// lexer.c
t_node					*split_into_tokens(char *s);
void					free_list(t_node *tokens, bool free_content);
t_node					*new_node(void *token);

// parser.c
t_node					*parse_tokens(t_node *tokens);

// utils.c
int						ft_isspace(int c);
int						ft_isalnum(int c);
char					*ft_strchr(const char *s, int c);
size_t					ft_strspn(const char *s, const char *accept);
size_t					ft_strcspn(const char *s, const char *reject);
int						ft_strncmp(const char *s1, const char *s2, size_t n);
char					*ft_strndup(const char *s1, size_t len);
void					*ft_memcpy(void *dst, const void *src, size_t n);
size_t					ft_strlen(const char *s);
char					*ft_strjoin(const char *s1, const char *s2);
void					*ft_realloc(void *ptr, size_t old_size, size_t size);

#endif