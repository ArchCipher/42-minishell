#include "minishell.h"

int type_redir(t_token_type t)
{
    return (t == REDIR_IN || t == REDIR_OUT || t == APPEND || t == HEREDOC);
}

int type_con(t_token_type t)
{
    return (t == PIPE_CHAR || t == AND || t == OR);
}

t_token *find_close_paren(t_token *token)
{
    int depth;

    depth = 0;
    while(token)
    {
        if (token->type == PAREN_O)
            depth++;
        else if (token->type == PAREN_C)
            depth--;
        if (!depth)
            return (token);
        token = token->next;
    }
}

t_cmd   *parse_cmds(t_token **tokens)
{
    t_cmd   *cmd;
    t_token *start;
    t_token *end;

    cmd = create_cmd(0);
    while(*tokens)
    {
        if ((*tokens)->type == PAREN_O)
        {
            start = (*tokens)->next;
            end = find_close_paren(*tokens);
            cmd->subshell = slice_cmds(start, end);
            *tokens = end->next;
        }
        else if ((*tokens)->type == WORD)
            cmd->args = collect_args(tokens);
        else if (is_connector((*tokens)->type))
        {
            cmd->con = (*tokens)->type;
            *tokens = (*tokens)->next;
            break;
        }
    }
}



static t_cmd	*create_cmd(size_t word_count)
{
	t_cmd	*new;

	new = malloc(sizeof(t_cmd));
	if (!new)
		return (perror(MINI), NULL);
	if (word_count)
	{
		new->args = (char **)malloc((word_count + 1) * sizeof(char *));
		if (!new->args)
			return (perror(MINI), free(new), NULL);
	}
	else
		new->args = NULL;
	new->redirs = NULL;
	new->next = NULL;
	new->exec.builtin = -1;
	new->exec.pid = -1;
	new->con = NONE;
	new->subshell = NULL;
	return (new);
}
