#include "minishell.h"

char    **copy_cmd(char **args, t_node *start, size_t word_count)
{
    size_t      i;

    i = 0;
    while (i < word_count)
    {
        args[i++] = start->token;
        start->token = NULL;
        start = start->next;
    }
    args[i] = NULL;
    return (args);
}

t_command   *new_cmd(t_node *word_start, size_t word_count)
{
    t_command *new;

    new = malloc(sizeof(t_command));
    if (!new)
        return (NULL);
    new->args = (char **) malloc((word_count + 1) * sizeof(char *));
    if (!new->args)
        return (free(new), NULL);
    new->args = copy_cmd(new->args, word_start, word_count);
    new->redirs = NULL;
    new->next = NULL;
    return (new);
}

void    free_cmds(t_command *cmds)
{
    t_command	*cmd;
	t_command	*tmp;
    size_t		i;

    cmd = cmds;
    while(cmd)
    {
        i = 0;
        while(cmd->args[i])
        {
            printf("cmd[%zu]: %s\n", i, cmd->args[i]);
            free(cmds->args[i++]);
        }
        free(cmd->args);
        tmp = cmd;
        cmd = cmd->next;
        free(tmp);
    }
}

t_command *build_ast(t_node *tokens)
{
    t_command   *cmds;
    t_command   *cur_cmd;
    t_command   *new;
    t_node      *current;
    t_node      *word_start;
    size_t      word_count;
    t_node      *redir_start;
    size_t      redir_count;
    

    if (!tokens)
        return (NULL);
    current = tokens;
    word_start = NULL;
    word_count = 0;
    cmds = NULL;
    while (current)
	{
        // printf("type: %d %s\n", current->type, current->token);
        if (current->type == word)
        {
            if (!word_start)
                word_start = current;
            word_count++;
            // printf("count: %zu\n", word_count);
        }
        else if (current->type != pipe_char)
        {
            
        }
        else
        {
            printf("test\n");
            if (word_start)
            {
                printf("test1\n");
                new = new_cmd(word_start, word_count);
                if (!new)
                    return (free_cmds(cmds), free_list(tokens, true), NULL);  // will leak!!! should free previous cmds
                if (!cmds)
                    cmds = new;
                else
                    cur_cmd->next = new;
                cur_cmd = new;
                word_start = NULL;
                word_count = 0;
            }
        }
        current = current->next;
	}
    if (word_start)
    {
        new = new_cmd(word_start, word_count);
        if (!new)
            return (free_cmds(cmds), free_list(tokens, true), NULL);  // will leak!!! should free previous cmds
        if (!cmds)
            cmds = new;
        else
            cur_cmd->next = new;
        cur_cmd = new;
        word_start = NULL;
        word_count = 0;
    }
    free_list(tokens, false);
	return (cmds);
}