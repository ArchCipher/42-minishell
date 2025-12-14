#include "minishell.h"

static t_cmd    *build_cmd(t_token **current);
static int      build_redir(t_token **current, t_list *cmd);
static t_cmd    *create_cmd(size_t word_count);
static t_redir  *create_redir(char *file, e_token_type type);

t_cmd *build_ast(t_token *tokens)
{
    t_list cmd;
    t_token *current;

    if (!tokens)
        return (NULL);
    current = tokens;
    cmd.head = NULL;
    while (current)
	{
        if (current->type != word)  // needed?
           return (error_free(cmd.head, tokens)); // print parse error? 
        cmd.new = build_cmd(&current);
        if (!cmd.new)
            return (error_free(cmd.head, tokens));
        lstadd_back((void **)&cmd.head, (void *)cmd.new, (void *)cmd.tail, TYPE_CMD);
        cmd.tail = cmd.new;
        if (!build_redir(&current, &cmd))
           return (error_free(cmd.head, tokens));
        if (current && current->type == pipe_char)
            current = current->next;
	}
    free_tokens(tokens, false);
	return (cmd.head);
}

static t_cmd   *build_cmd(t_token **current)
{
    t_cmd   *new;
    t_token *tmp;
    size_t  word_count;
    size_t  i;

    tmp = *current;
    word_count = 0;
    while(tmp && tmp->type == (*current)->type)
    {
        word_count++;
        tmp = tmp->next;
    }
    new = create_cmd(word_count);
    if (!new)
        return (NULL);
    i = 0;
    while (i < word_count)
    {
        new->args[i++] = (*current)->token;
        (*current)->token = NULL;
        *current = (*current)->next;
    }
    new->args[i] = NULL;
    return (new);
}

static int build_redir(t_token **current, t_list *cmd)
{
    t_list redir;

    while (*current && (*current)->type != pipe_char)
    {
        if ((*current)->next && (*current)->next->type != word)    // print parse error
            return (0);
        free((*current)->token);
        (*current)->token = NULL;
        redir.new = create_redir((*current)->next->token, (*current)->type);
        if (!redir.new)
            return (0);
        lstadd_back((void **)&((t_cmd *)cmd->new)->redirs, (void *)redir.new, (void *)redir.tail, TYPE_REDIR);
        redir.tail = redir.new;
        (*current) = (*current)->next->next;
    }
    return (1);
}

static t_cmd   *create_cmd(size_t word_count)
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

static t_redir *create_redir(char *file, e_token_type type)
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
