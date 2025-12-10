// echo "hello" text.txt should print "hello" text.txt

#include "minishell.h"

void print_pwd()
{
    char *buf = NULL;

    buf = getcwd(buf, 0);
    if (!buf)
        return (perror("Error: "));
    printf("%s\n", buf);
    free(buf);
}


void print_cd(const char *path)
{
    if (chdir(path) == -1)
        perror("Error: ");
}
int end_quote(char *s, char c)
{
    while(*s)
    {
        if (*s == c)
            return (1);
        s++;
    }
    return (0);
}

void free_plist(t_node *p_tokens)
{
    t_node *current;

    while (p_tokens)
    {
        current = p_tokens;
        p_tokens = p_tokens->next;
        free(current->token);
        free(current);
    }
}
/*
expand_vars(char *token)
{

}
*/
char *handle_word(char *token, size_t t_len)
{
    char *new;
    char *current;
    // char *quote;
    size_t len;
    enum e_token_type flag;
    
    current = token;
    flag = word;
    len = 0;
    new = NULL;
    while(*current && t_len--)
    {
        if (flag == equote || flag == dollar_sign)
            flag = word;
        if (*current == '\'' && flag != squote && end_quote(current + 1, '\''))
            flag = squote;
        else if (*current == '\"' && flag != dquote && end_quote(current + 1, '\"'))
            flag = dquote;
        else if ((*current == '\'' && flag == squote) || (*current == '\"' && flag == dquote))
            flag = equote;
        else if (*current == '$' && flag != squote)
        {
            flag = dollar_sign;
            // expand_var(&current);
        }
        if (flag != equote && flag != dollar_sign && !(*current == '\'' && flag == squote) && !(*current == '\"' && flag == dquote))
        {
            // printf("char %c %d\n", *current, flag);
            if (!new)
                new = malloc(2);
            else
                new = realloc(new, len + 2); //redo!!!
            new[len] = *current;
            len++;
            new[len] = 0;
        }
        current++;
    }
    printf("token: %s\n", new);
    return (new);
}

t_node *parse_tokens(t_node *tokens)
{
    t_node *p_tokens;
    t_node *current;
    t_node *p_current;
    t_node *new;
    char *token;

    current = tokens;
    p_tokens = NULL;
    while (current)
    {
        if (current->type == word)
            token = handle_word(current->token, current->len);
        else
            token = ft_strndup(current->token, current->len);
        if (!token)
            return (free_plist(p_tokens), NULL);
        new = new_node(token);
        if (!new)
            return (free_plist(p_tokens), NULL);
        if (!p_tokens)
            tokens = new;
        else
            p_current->next = new;
        p_current = new;
        current = current->next;
    }
    return (p_tokens);
}

/*
void echo(t_node *current)
{
    bool nl;

    nl = true;
    current = current->next;
    if (ft_strncmp(current->token, "-n", 2) == 0)
    {
        nl = false;
        current = current->next;
    }
    while (current && current->type == word)
    {
        // handle quotes, env vars etc
        if (*(current->token) == '\'' || *(current->token) == '\"')
        {
            (current->token)++;
            current->len -= 2;
        }
        write(1, current->token, current->len);
        current = current->next;
    }
    if (nl == true)
            write(1, "\n", 1);
}

void parse_tokens(t_node *tokens)
{
    t_node *current;

    current = tokens;
    while (current)
    {
        if (ft_strncmp(current->token, "echo", 4) == 0)
            echo(current);
        else if (ft_strncmp(current->token, "pwd", 3) == 0)
            pwd();
        else if(ft_strncmp(current->token, "exit", 4) == 0)
            exit(0);
        current = current->next;
    }
}
*/