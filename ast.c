#include "minishell.h"

t_cmd   *create_new_cmd(size_t word_count)
{
    t_cmd *new;

    new = malloc(sizeof(t_cmd));
    if (!new)
        return (NULL);
    new->args = (char **) malloc((word_count + 1) * sizeof(char *));
    if (!new->args)
        return (free(new), NULL);
    new->redirs = NULL;
    new->next = NULL;
    return (new);
}

void    free_cmds(t_cmd *cmds)
{
    t_cmd	*cmd;
	t_cmd	*tmp;
    t_redir *cur_redir;
    size_t		i;

    cmd = cmds;
    while(cmd)
    {
        i = 0;
        while(cmd->args[i])
        {
            printf("cmd[%zu]: %s\n", i, cmd->args[i]);
            free(cmd->args[i++]);
        }
        free(cmd->args);
        while (cmd->redirs)
        {
            printf("redir file: %s flag: %d\n", cmd->redirs->file, cmd->redirs->flag);
            cur_redir = cmd->redirs;
            cmd->redirs = cmd->redirs->next;
            free(cur_redir->file);
        }
        tmp = cmd;
        cmd = cmd->next;
        free(tmp);
    }
}

size_t  count_tokens(t_token *current, enum e_token_type type)
{
    size_t      count;

    count = 0;
    while(current && current->type == type)
    {
        count++;
        current = current->next;
    }
    return (count);
}

char    **copy_cmd(char **args, t_token **start, size_t word_count)
{
    size_t      i;

    i = 0;
    while (i < word_count)
    {
        args[i++] = (*start)->token;
        (*start)->token = NULL;
        *start = (*start)->next;
    }
    args[i] = NULL;
    return (args);
}

t_redir *create_new_redir(char *file, enum e_token_type type)
{
    t_redir *new;

    new = malloc(sizeof(t_redir));
    if (!new)
        return (NULL);
    new->file = file;
    new->flag = type;
    new->next = NULL;
    return (new);
}

t_cmd *build_ast(t_token *tokens)
{
    t_cmd   *cmds;
    t_cmd   *new_cmd;
    t_cmd   *cur_cmd;
    t_redir *new_redir;
    t_redir *cur_redir;
    t_token *current;
    size_t  word_count;

    if (!tokens)
        return (NULL);
    current = tokens;
    cmds = NULL;
    while (current)
	{
        // printf("type: %d %s\n", current->type, current->token);
        if (current->type != word)  // needed?
           return (free_cmds(cmds), free_list(tokens, true), NULL); // print parse error? 
        word_count = count_tokens(current, current->type);
        printf("count: %zu\n", word_count);
        new_cmd = create_new_cmd(word_count);
        if (!new_cmd)
            return (free_cmds(cmds), free_list(tokens, true), NULL);
        new_cmd->args = copy_cmd(new_cmd->args, &current, word_count);
        if (!cmds)
            cmds = new_cmd;
        else
            cur_cmd->next = new_cmd;
        cur_cmd = new_cmd;
        while (current && current->type != pipe_char)
        {
            printf("token: %s next: %s\n", current->token, current->next->token);
            if (current->next->type != word)    // print parse error
                return (free_cmds(cmds), free_list(tokens, true), NULL);
            free(current->token);
            current->token = NULL;
            new_redir = create_new_redir(current->next->token, current->type);
            if (!new_redir)
                return (free_cmds(cmds), free_list(tokens, true), NULL);
            if (!new_cmd->redirs)
                new_cmd->redirs = new_redir;
            else
                cur_redir->next = new_redir;
            cur_redir = new_redir;
            current = current->next->next;
        }
        if (current && current->type == pipe_char)
            current = current->next;
	}
    free_list(tokens, false);
	return (cmds);
}