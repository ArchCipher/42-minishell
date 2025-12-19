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

void    free_cmds(t_cmd *cmd)
{
	t_cmd	*tmp;
    t_redir *cur_redir;
    size_t	i;
    t_cmd   *cmds;

    cmds = cmd;
    while(cmds)
    {
        i = 0;
        while(cmds->args[i])
            free(cmds->args[i++]);
        free(cmds->args);
        while (cmds->redirs)
        {
            cur_redir = cmds->redirs;
            cmds->redirs = cmds->redirs->next;
            free(cur_redir->file);
            free(cur_redir);
        }
        tmp = cmds;
        cmds = cmds->next;
        free(tmp);
    }
}

t_cmd    *error_free(t_cmd *cmds, t_token *tokens)
{
    free_cmds(cmds);
    free_tokens(tokens, true, NULL);
    return (NULL);
}
