#include "minishell.h"

void	free_tokens(t_token *tokens, bool free_content, t_token *end)
{
	t_token	*current;

	while (tokens && tokens != end)
	{
		current = tokens;
		tokens = tokens->next;
		if (free_content && current->type == word && current->token)
			free(current->token);
		free(current);
	}
}

void    free_cmds(t_cmd *cmds)
{
	t_cmd	*tmp;
    t_redir *cur_redir;
    size_t	i;
    t_cmd   *cmd;

    cmd = cmds;
    while(cmd)
    {
        i = 0;
        while(cmd->args[i])
            free(cmd->args[i++]);
        free(cmd->args);
        while (cmd->redirs)
        {
            cur_redir = cmd->redirs;
            cmd->redirs = cmd->redirs->next;
            free(cur_redir->file);
            if (cur_redir->fd != -1)
                close(cur_redir->fd);
            free(cur_redir);
        }
        tmp = cmd;
        cmd = cmd->next;
        free(tmp);
    }
}

t_cmd    *error_free(t_cmd *cmds, t_token *tokens)
{
    free_cmds(cmds);
    free_tokens(tokens, true, NULL);
    return (NULL);
}
