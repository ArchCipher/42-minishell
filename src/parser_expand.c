/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_expand.c                                     :+:      :+:    :+:  */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 14:41:34 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/11 17:19:20 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	expand_word_to_args(t_expand s, char ***args, size_t *cap,
				size_t *len);
static void	expand_remove_quote(t_expand *s, t_shell *shell);

/*
DESCRIPTION:
	Expands variables and tilde in arguments and allocates new strings
	for the expanded results. Returns 0 on success, 1 on error.
*/

int	expand_args(t_cmd *cmd, t_shell *shell)
{
	t_expand	s;
	char		**new_args;
	size_t		i;
	size_t		j;

	new_args = ft_calloc(cmd->argcap + 1, sizeof(char *));
	if (!new_args)
		return (perror(MINI), 1);
	i = 0;
	j = 0;
	while (cmd->args[i])
	{
		s = expand_word(cmd->args[i], shell);
		if (expand_word_to_args(s, &new_args, &cmd->argcap, &j))
			return (free(s.dst), free_arr(new_args), 1);
		i++;
	}
	free_arr(cmd->args);
	cmd->args = new_args;
	cmd->arglen = j;
	new_args[j] = NULL;
	return (0);
}

t_expand	expand_word(char *src, t_shell *shell)
{
	t_expand	s;

	s = init_expand(src, ft_strlen(src));
	if (!s.dst)
		return (perror(MINI), s);
	expand_remove_quote(&s, shell);
	if (!s.dst)
		return (perror(MINI), s);
	s.has_space = (ft_strlen(s.dst) != ft_strcspn(s.dst, IS_SPACE));
	s.quoted = (ft_strlen(src) != ft_strcspn(src, "\'\""));
	return (s);
}

static int	expand_word_to_args(t_expand s, char ***args, size_t *cap,
		size_t *len)
{
	if (!s.dst)
		return (1);
	if (!s.quoted && !*s.dst)
		return (free(s.dst), 0);
	if (!s.expanded || !s.has_space || (s.quoted && !s.expanded))
		return (append_arg(args, cap, len, s.dst));
	return (split_args(args, cap, len, s.dst));
}

static void	expand_remove_quote(t_expand *s, t_shell *shell)
{
	t_token_type	flag;

	flag = WORD;
	while (s->src < s->src_end)
	{
		if ((*(s->src) == '\'' && (flag == WORD || flag == SQUOTE))
			|| (*(s->src) == '\"' && (flag == WORD || flag == DQUOTE)))
			flag = update_quote_flag(*(s->src), flag);
		else if (dollar_expandable(s->src, s->src_end) && flag != SQUOTE)
		{
			if (flag == WORD)
				s->expanded = true;
			if (!append_var(s, shell))
				continue ;
			free(s->dst);
			s->dst = NULL;
			return ;
		}
		else if (!(*(s->src) == '$' && flag == WORD && s->src + 1 < s->src_end
				&& (!ft_strcspn(s->src + 1, "\'\""))))
			s->dst[s->len++] = *(s->src);
		s->src++;
	}
	s->dst[s->len] = 0;
}
