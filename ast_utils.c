#include "minishell.h"

void    free_cmds(t_cmd *cmds)
{
	t_cmd	*tmp;
    t_redir *cur_redir;
    size_t	i;

    while(cmds)
    {
        i = 0;
        while(cmds->args[i])
        {
            printf("cmd[%zu]: %s\n", i, cmds->args[i]);
            free(cmds->args[i++]);
        }
        free(cmds->args);
        while (cmds->redirs)
        {
            printf("redir: %s %d\n", cmds->redirs->file, cmds->redirs->flag);
            cur_redir = cmds->redirs;
            cmds->redirs = cmds->redirs->next;
            free(cur_redir->file);
        }
        tmp = cmds;
        cmds = cmds->next;
        free(tmp);
    }
}

t_cmd    *error_free(t_cmd *cmds, t_token *tokens)
{
    free_cmds(cmds);
    free_tokens(tokens, true);
    return (NULL);
}
