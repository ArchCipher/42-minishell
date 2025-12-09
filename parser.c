// echo "hello" text.txt should print "hello" text.txt

#include "minishell.h"
#include <string.h>

void echo(t_node *current)
{
    bool nl;

    nl = true;
    current = current->next;
    if (strncmp(current->token, "-n", 2) == 0)
    {
        nl = false;
        current = current->next;
    }
    if (current)
    {
        // handle quotes, env vars etc
        write(1, current->token, current->len);
        if (nl == true)
            write(1, "\n", 1);
    }
}

void parse_tokens(t_node *tokens)
{
    t_node *current;

    current = tokens;
    while (current)
    {
        if (strncmp(current->token, "echo", 4) == 0)
            echo(current);
        else if(strncmp(current->token, "exit", 4) == 0)
            exit(0);
        current = current->next;
    }
}