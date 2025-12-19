#include "minishell.h"

static t_cmd    *build_cmd(t_token **current);
static int      build_redir(t_token **current, t_redir **head, t_list *redir);
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
    cmd.new = NULL;
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
        else if (token->next && token->next->type == word)
            token = token->next;
        else
            return (printf("%s: %s`%c'\n", MINI, E_PARSE, token->next->token[0]), -1);
        token = token->next;
    }
    return (word_count);
}

static t_cmd   *build_cmd(t_token **current)
{
    t_cmd   *new;
    t_list  redir;
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
        else if (!build_redir(current, &new->redirs, &redir))
        {
            new->args[i] = NULL;
            free_cmds(new);
            return (NULL);
        }
    }
    new->args[i] = NULL;
    return (new);
}

static int build_redir(t_token **current, t_redir **head, t_list *redir)
{
    while (*current && (*current)->type != pipe_char && (*current)->type != word)
    {
        redir->new = create_redir((*current)->next->token, (*current)->type);
        if (!redir->new)
            return (perror(MINI), 0);
        lstadd_back((void **)head, (void *)redir->new, (void *)redir->tail, TYPE_REDIR);
        redir->tail = redir->new;
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
