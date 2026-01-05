#include "minishell.h"

static int	build_redir(t_token **tok, void **head, void **last);
static int	alloc_cmd_args(t_token *tok, t_cmd *cmd);

/*
DESCRIPTION:
	Builds an entire command struct with arguments and redirections.
	Word tokens following redir are added as redir file or delimeter(heredoc).
	Other word tokens are added to args.
	Old word tokens are made null, to avoid double free on error.
	Returns the newly built command struct on success, NULL on error.
*/

int	build_cmd(t_token **tok, t_cmd *cmd, void **last_redir)
{
	ssize_t	i;

	i = 0;
	if (!alloc_cmd_args(*tok, cmd))
		return (0);
	while (*tok && ((*tok)->type == WORD || is_type_redir((*tok)->type)))
	{
		if ((*tok)->type == WORD)
		{
			cmd->args[i++] = (*tok)->word;
			(*tok)->word = NULL;
			*tok = (*tok)->next;
		}
		else if (is_type_redir((*tok)->type)
			&& !build_redir(tok, (void **)&cmd->redirs, last_redir))
		{
			if (cmd->args)
				cmd->args[i] = NULL;
			return (0);
		}
	}
	return (1);
}

/*
DESCRIPTION:
	Builds redirect struct redirection token and following word token.
	Word token following redirection token is treated as delimeter for heredoc,
	and file for others.
	Old word tokens are made null, to avoid double free on error.
*/

static int	build_redir(t_token **tok, void **head, void **last)
{
	t_redir	*new;

	while (*tok && is_type_redir((*tok)->type) && (*tok)->next
		&& (*tok)->next->type == WORD)
	{
		new = malloc(sizeof(t_redir));
		if (!new)
			return (perror(MINI), 0);
		new->file = (*tok)->next->word;
		(*tok)->next->word = NULL;
		new->flag = (*tok)->type;
		new->fd = -1;
		new->quoted = (*tok)->next->quoted;
		new->next = NULL;
		lstadd_back(head, (void *)new, *last, TYPE_REDIR);
		*last = new;
		*tok = (*tok)->next->next;
	}
	if (*tok && is_type_redir((*tok)->type) && !((*tok)->next
			&& (*tok)->next->type == WORD))
		return (perr_msg("build_redir", "parsing error", NULL, false), 0);
	return (1);
}

/*
DESCRIPTION:
	Counts words until it reaches end of cmd / pipe character.
	Returns the number of words found.
	Although only defensive, returns -1 on leaks in the parser.
*/

static int	alloc_cmd_args(t_token *tok, t_cmd *cmd)
{
	ssize_t	word_count;

	word_count = 0;
	while (tok && (tok->type == WORD || is_type_redir(tok->type)))
	{
		if (tok->type == WORD)
			word_count++;
		else if (is_type_redir(tok->type))
			tok = tok->next;
		tok = tok->next;
	}
	if (!word_count)
		return (1);
	cmd->args = malloc(sizeof(char *) * (word_count + 1));
	if (!cmd->args)
		return (perror(MINI), 0);
	cmd->args[word_count] = NULL;
	return (1);
}
