#include "minishell.h"

static t_cmd    *build_cmd(t_token **current);
static int      build_redir(t_token **current, t_redir **head, t_redir **last);
static t_cmd    *create_cmd(size_t word_count);

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
        if(current->type != pipe_char)
        {
            cmd.new = build_cmd(&current);
            if (!cmd.new)
                return (error_free(cmd.head, tokens));
            lstadd_back((void **)&cmd.head, (void *)cmd.new, (void *)cmd.tail, TYPE_CMD);
            cmd.tail = cmd.new;
        }
        if (current && current->type == pipe_char)
        {
            current = current->next;
            if (current->type != word)
                return (perror("MINI"), error_free(cmd.head, tokens));   // print parse error 
        }
	}
    free_tokens(tokens, false, NULL);
	return (cmd.head);
}

/*
token->next && token->next->type == word:
when token is an operator and next token is a word, skips counting operator and word after it
else if
when token is an operator and next token is not word, prints parse error
*/

ssize_t count_args(t_token *token)
{
    ssize_t word_count;

    word_count = 0;
    while(token && token->type != pipe_char)
    {
        if (token->type == word)
            word_count++;
        else if (token->next && token->next->type == word) // can be else
            token = token->next;
        token = token->next;
    }
    return (word_count);
}

static t_cmd   *build_cmd(t_token **current)
{
    t_cmd   *new;
    t_redir *last;
    ssize_t  word_count;
    ssize_t  i;

    if ((word_count = count_args(*current)) == -1)
        return (NULL);
    new = create_cmd(word_count);
    if (!new)
        return (perror(MINI), NULL);
    i = 0;
    while (*current && (*current)->type != pipe_char)
    {
        if ((*current)->type == word)
        {
            new->args[i++] = (*current)->token;
            (*current)->token = NULL;
            *current = (*current)->next;
        }
        else if (!build_redir(current, &new->redirs, &last))
        {
            new->args[i] = NULL;
            free_cmds(new);
            return (NULL);
        }
    }
    new->args[i] = NULL;
    return (new);
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
    new->exec.builtin = -1;
    new->exec.pid = -1;
    return (new);
}

static int build_redir(t_token **current, t_redir **head, t_redir **last)
{
    t_redir *new;

    while (*current && (*current)->type != pipe_char && (*current)->type != word)
    {
        new = malloc(sizeof(t_redir));
        if (!new)
            return (perror(MINI), 0);
        new->file = (*current)->next->token;
        new->flag = (*current)->type;
        new->fd = -1;
        new->next = NULL;
        lstadd_back((void **)head, (void *)new, (void *)*last, TYPE_REDIR);
        *last = new;
        *current = (*current)->next->next;
    }
    return (1);
}
