/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_expand.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 14:41:34 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 17:36:27 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_remove_quote(char *src, bool *quoted, t_shell *shell);

// 	if (cur_tok->type != WORD)
// 		return (0);
// 	if (expand_word_token(cur_tok, prev_tok, shell))
// 		return (1);
// 	if (cur_tok->quoted || !cur_tok->word)
// 		return (0);
// 	if (!cur_tok->quoted && is_target_fd(cur_tok, get_tok((*cur)->next)))
// 		return (0);
// 	if (!*cur_tok->word)
// 		return (del_one_token(tokens, prev, cur), 2);
// 	if (ft_strlen(cur_tok->word) == ft_strcspn(cur_tok->word, IS_SPACE))
// 		return (0);
// 	if (prev_tok && is_type_redir(prev_tok->type))
// 		return (perr_tok_msg(NULL, cur_tok->raw, cur_tok->len, E_REDIR), 1);
// 	return (split_word_token(tokens, cur, prev));
// }

/*
DESCRIPTION:
	Splits a word token containing spaces into multiple word tokens.
	The original token is deleted and replaced with new tokens created from
	the space-separated words. Returns 0 on success, 1 on failure.
*/

int	split_args(t_cmd *cmd, ssize_t *j, char *s)
{
	char	*str_tok;
	char	*p;
	char	*tmp;
	char	**tmp_args;

	str_tok = ft_strtok_r(s, IS_SPACE, &p);
	while (str_tok)
	{
		tmp = ft_strdup(str_tok);
		if (!tmp)
			return (1);
		if (cmd->argcap < (*j) + 1)
		{
			tmp_args = ft_realloc(cmd->args, cmd->argcap * sizeof(char *),
					(cmd->argcap + 1) * sizeof(char *));
			if (!tmp_args)
				return (free(tmp), 1);
			cmd->args = tmp_args;
			cmd->argcap++;
		}
		cmd->args[(*j)++] = tmp;
		str_tok = ft_strtok_r(NULL, IS_SPACE, &p);
	}
	free(s);
	return (0);
}

/*
DESCRIPTION:
	Expands variables and tilde in a word token and allocates a new string
	for the expanded result. Returns 0 on success, 1 on error.
*/

int	expand_args(t_cmd *cmd, t_shell *shell)
{
	ssize_t	i;
	ssize_t	j;
	char	*tmp;
	bool	quoted;

	i = 0;
	j = 0;
	while (i < cmd->arglen)
	{
		quoted = (ft_strchr(cmd->args[i], '\'') || ft_strchr(cmd->args[i], '"'));
		if (quoted || ft_strchr(cmd->args[i], '$'))
		{
			tmp = expand_remove_quote(cmd->args[i], &quoted, shell);
			if (!tmp)
				return (1);
			if (!quoted && !*tmp)
				free(tmp);
			else if ((!quoted && ft_strlen(tmp) == ft_strcspn(tmp, IS_SPACE)) || quoted)
				cmd->args[j++] = tmp;
			else if (split_args(cmd, &j, tmp))
				return (free(tmp), 1);
		}
		else
			cmd->args[j++] = cmd->args[i];
		i++;
	}
	cmd->args[j] = NULL;
	return (0);
}

char	*expand_str(char *src, t_shell *shell, bool *quoted, bool split)
{
	char	*dst;

	*quoted = false;
	if (ft_strlen(src) == ft_strcspn(src, "\'\"$"))
		return (src);
	dst = expand_remove_quote(src, quoted, shell);
	if (!dst)
		return (NULL);
	if (!split && *quoted)
	{
		perr_msg(src, E_REDIR, NULL, false);
		free(dst);
		return (NULL);
	}
	free(src);
	return (dst);
}

char	*expand_remove_quote(char *src, bool *quoted, t_shell *shell)
{
	t_expand		s;
	t_token_type	flag;

	s = init_expand(src, ft_strlen(src));
	if (!s.dst)
		return (perror(MINI), NULL);
	flag = WORD;
	while (s.src < s.src_end)
	{
		if ((*s.src == '\'' && (flag == WORD || flag == SQUOTE))
				|| (*s.src == '\"' && (flag == WORD || flag == DQUOTE)))
			flag = update_quote_flag(*s.src, flag);
		else if (dollar_expandable(s.src, s.src_end) && flag != SQUOTE)
		{
			if (flag == WORD)
				*quoted = true;
			if (append_var(&s, shell))
				return (free(s.dst), NULL);
			continue ;
		}
		else
			s.dst[s.len++] = *s.src;
		s.src++;
	}
	s.dst[s.len] = 0;
	return (s.dst);
}

/*
DESCRIPTION:
	Copies src to dst while removing quote characters.
	Returns pointer to the null terminator in dst.
*/

char	*remove_quotes(char *dst, const char *src)
{
	t_token_type	old_flag;
	t_token_type	flag;

	old_flag = WORD;
	while (*src)
	{
		flag = update_quote_flag(*src, old_flag);
		if (flag == old_flag)
			*dst++ = *src++;
		else
			src++;
		old_flag = flag;
	}
	*dst = 0;
	return (dst);
}
