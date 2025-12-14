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
            printf("cmd: %s\n", cmd->args[i]);
            free(cmd->args[i++]);
        }
        free(cmd->args);
        while (cmd->redirs)
        {
            printf("redir: %s %d\n", cmd->redirs->file, cmd->redirs->flag);
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

void copy_cmd(char **args, t_token **start, size_t word_count)
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

// void	cmdadd_back(t_cmd **cmds, t_cmd *new, t_cmd *last)
// {
// 	if (!cmds || !new)
// 		return ;
// 	if (!*cmds)
// 		*cmds = new;
// 	else
// 		last->next = new;
// }

// void	rediradd_back(t_redir **redirs, t_redir *new, t_redir *last)
// {
// 	if (!redirs || !new)
// 		return ;
// 	if (!*redirs)
// 		*redirs = new;
// 	else
// 		last->next = new;
// }

t_cmd    *error_free(t_cmd *cmds, t_token *tokens)
{
    free_cmds(cmds);
    free_list(tokens, true);
    return (NULL);
}

t_cmd *build_ast(t_token *tokens)
{
    u_node new_union;
    t_cmd   *cmds;
    // t_cmd   *new_cmd;
    t_cmd   *cur_cmd;
    // t_redir *new_redir;
    t_redir *cur_redir;
    t_token *current;
    size_t  word_count;

    if (!tokens)
        return (NULL);
    current = tokens;
    cmds = NULL;
    while (current)
	{
        if (current->type != word)  // needed?
           return (error_free(cmds, tokens)); // print parse error? 
        word_count = count_tokens(current, current->type);
        new_union.cmd = create_new_cmd(word_count);
        if (!new_union.cmd)
            return (error_free(cmds, tokens));
        copy_cmd(new_union.cmd->args, &current, word_count);
        lstadd_back((void **)&cmds, new_union.cmd, (void *)cur_cmd, TYPE_CMD);
        cur_cmd = new_union.cmd;
        while (current && current->type != pipe_char)
        {
            if (current->next->type != word)    // print parse error
                return (free_cmds(cmds), free_list(tokens, true), NULL);
            free(current->token);
            current->token = NULL;
            new_union.redir = create_new_redir(current->next->token, current->type);
            if (!new_union.redir)
                return (error_free(cmds, tokens));
            lstadd_back((void **)&new_union.cmd->redirs, new_union.redir, (void *)cur_redir, TYPE_REDIR);
            cur_redir = new_union.redir;
            current = current->next->next;
        }
        if (current && current->type == pipe_char)
            current = current->next;
	}
    free_list(tokens, false);
	return (cmds);
}
