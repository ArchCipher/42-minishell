/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 13:47:52 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:53:15 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_word(t_token *token, char *end, t_shell *shell);
static void	handle_char(char *s, e_token_type *flag, char *end, t_string *str);
static int	del_one_token(t_token **tokens, t_token **cur, t_token *prev);
static void	error_free_tokens(t_token *tokens, t_token *current);

/*
DESCRIPTION:
	Parses the tokens and returns the parsed tokens.
	Mallocates a new string for only word tokens, others are pointers to the
	input strings with just the token->type set.
	Returns the parsed tokens on success, NULL on error.

NOTE:
	Now returns error for logical OR (||).
*/

t_token	*parse_tokens(t_token *tokens, t_shell *shell)
{
	t_token	*current;
	t_token	*prev;

	if (!tokens)
		return (NULL);
	current = tokens;
	prev = NULL;
	while (current)
	{
		if (current->type == word)
		{
			current->quoted = (ft_memchr(current->token, '\'', current->len) 
				|| ft_memchr(current->token, '\"', current->len));
			if (handle_word(current, current->token + current->len, shell))
				return (free_tokens(tokens, true, current), NULL);
			if ((!prev && !*current->token) && del_one_token(&tokens, &current, prev))
				continue ;
		}
		else if (!current->next || (current->type == pipe_char && current->next->type == pipe_char) 
					|| (current->type != pipe_char && current->next->type != word))
			return (error_free_tokens(tokens, current), NULL);
		prev = current;
		current = current->next;
	}
	return (tokens);
}

/*
DESCRIPTION:
	Handles the word token and returns 1 if successful, 0 on error.
*/

static int handle_word(t_token *tok, char *end, t_shell *shell)
{
	e_token_type	flag;
	t_string		str;

	str.s = malloc(tok->len + 1);
	if (!str.s)
		return (perror(MINI), 1);
	str.cap = tok->len;
	str.len = 0;
	flag = word;
	while (tok->token < end)
	{
		handle_char(tok->token, &flag, end, &str);
		if (dollar_expandable(tok->token, end) && flag != squote)
		{
			tok->token++;
			if (append_var(&str, get_var(&tok->token, end, shell), end - tok->token))
				return (free(str.s), 1);
		}
		else
			tok->token++;
	}
	str.s[str.len] = 0;
	tok->token = str.s;
	return (0);
}

/*
DESCRIPTION:
	Appends a character to the string unless it is a quote or an expandable dollar sign.
	In case of quotes, it updatesthe flag to the appropriate type.
*/

static void	handle_char(char *s, e_token_type *flag, char *end, t_string *str)
{
	if (*s == '\'' && *flag == word)
		*flag = squote;
	else if (*s == '\"' && *flag == word)
		*flag = dquote;
	else if ((*s == '\'' && *flag == squote) || (*s == '\"' && *flag == dquote))
		*flag = word;
	else if (!(dollar_expandable(s, end) && *flag != squote))
		str->s[(str->len)++] = *s;
}

/*
DESCRIPTION:
	Deletes the current token and returns 1.
*/

static int del_one_token(t_token **tokens, t_token **cur, t_token *prev)
{
	t_token *tmp;

	tmp = *cur;
	*cur = tmp->next;
	free(tmp->token);
	if (!prev)
		*tokens = tmp->next;
	else
		prev->next = tmp->next;
	free(tmp);
	return (1);
}

/*
DESCRIPTION:
	Prints the error message and frees the tokens.
*/

static void	error_free_tokens(t_token *tokens, t_token *current)
{
	if (!current->next)
		ft_dprintf(STDERR_FILENO, "%s: %s`newline'\n", MINI, E_PARSE);
	else
		ft_dprintf(STDERR_FILENO, "%s: %s`%c'\n", MINI, E_PARSE, current->next->token[0]);	
	free_tokens(tokens, true, current);
}
