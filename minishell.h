/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:33:15 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/04 18:38:40 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdbool.h>		// bool
// # include <readline/history.h>  // readline
# include <readline/readline.h> // readline
# include <stdio.h>             // printf, readline     //, strerror, perror
# include <stdlib.h>            // free      // malloc, free, exit, getenv
# include <unistd.h>

# define BUFLEN 256 
# define REJECT " \f\n\r\t\v<>|"
# define OPERATORS "<>|"

enum e_token_type
{
	word,
    squote,
    dquote,
	equote,
	operator,
	pipe_char,
	redirect_in,
	redirect_out,
	append,
	heredoc,
	dollar_sign
	// assignment
};

typedef struct s_node
{
	enum e_token_type	type;
	char				*token;
	size_t				len;
	struct s_node		*next;
}						t_node;

// lexer.c
t_node	*split_into_tokens(char *s);
void    free_list(t_node *tokens);
t_node	*new_node(void *token);

t_node	*parse_tokens(t_node *tokens);
void free_plist(t_node *p_tokens);

// utils.c
int		ft_isspace(int c);
char	*ft_strchr(const char *s, int c);
size_t	ft_strspn(const char *s, const char *accept);
size_t	ft_strcspn(const char *s, const char *reject);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_strndup(const char *s1, size_t len);

#endif