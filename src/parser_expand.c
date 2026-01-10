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

static char	*expand_remove_quote(char *src, bool *quoted, t_shell *shell);
static int	split_args(t_cmd *cmd, ssize_t *j, char *s);

/*
DESCRIPTION:
	Expands variables and tilde in arguments and allocates new strings
	for the expanded results. Returns 0 on success, 1 on error.
*/

int	expand_args(t_cmd *cmd, t_shell *shell)
{
	ssize_t	i;
	ssize_t	j;
	char	*tmp;
	bool	expand;
	bool	quoted;

	i = 0;
	j = 0;
	while (i < cmd->arglen)
	{
		quoted = (ft_strlen(cmd->args[i]) != ft_strcspn(cmd->args[i], "\'\""));
		tmp = expand_str(cmd->args[i], shell, &expand, true);
		if (!tmp)
			return (1);
		if (!quoted && !*tmp)
			free(tmp);
		else if ((!expand && ft_strlen(tmp) == ft_strcspn(tmp, IS_SPACE))
			|| quoted)
			cmd->args[j++] = tmp;
		else if (split_args(cmd, &j, tmp))
			return (free(tmp), 1);
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

static char	*expand_remove_quote(char *src, bool *quoted, t_shell *shell)
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
	return (s.dst[s.len] = 0, s.dst);
}

/*
DESCRIPTION:
	Splits a word token containing spaces into multiple word tokens.
	The original token is deleted and replaced with new tokens created from
	the space-separated words. Returns 0 on success, 1 on failure.
*/

static int	split_args(t_cmd *cmd, ssize_t *j, char *s)
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
		if (cmd->argcap <= (*j) + 1)
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
