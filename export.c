# include "minishell.h"

static t_env	*create_env(char *envp);
int      is_valid_identifier(char *s);

/*
export FORMAT:
    declare -x KEY="value"
declare -x PWD="/Users/kiru/Documents/CS/42_mygit/minishell"
declare -x SHELL="/bin/zsh"

print in alpabetical order

//check for invalid entries
starting with number, '-', no key etc.

export	0 / 1 depending on validity
*/

int exec_export(t_shell *shell, char **args)
{

    if (!*args)
        return (export_no_args(shell->env));
    while(*args)
    {
        if (!is_valid_identifier(*args))
            return (ft_dprintf(STDERR_FILENO, "%s: export: `%s': %s\n",MINI, *args, E_EXPORT), 1);
        if (update_env(shell, *args, env_lookup(shell->env, *args)))
            return (1);
        args++;
    }
    return (0);
}

int update_env(t_shell *shell, char *arg, t_env *env)
{
    char    *equal;

    if (!env)
    {
        env = create_env(arg);
        if (!env)
            return (1);
        lstadd_back((void **)&shell->env, env, shell->env_tail, TYPE_ENV);
        shell->env_tail = env;
        return (0);
    }
    equal = ft_memchr(arg, '=', ft_strlen(arg));
    if (equal)
    {
        if (env->value)
            free(env->value);
        env->value = ft_strndup(equal + 1, ft_strlen(equal + 1));
        if (!env->value)
            return (perror(MINI), 1);
    }
    env->exported = true;
    return (0);  
}

t_env *env_lookup(t_env *env, const char *arg)
{
    t_env   *prev;
    return (env_lookup_prev(env, &prev, arg));
}

static t_env	*create_env(char *envp)
{
	t_env	*new;
    char    *equal;

	new = malloc(sizeof(t_env));
	if (!new)
		return (perror(MINI), NULL);
    new->value = NULL;
    new->exported = false;
    equal = ft_memchr(envp, '=', ft_strlen(envp));
    if (equal)
    {
        new->key = ft_strndup(envp, equal - envp);
        new->exported = true;
    }    
    else
        new->key = ft_strndup(envp, ft_strlen(envp));
    if (!new->key)
        return (perror(MINI), free(new), NULL);
    if (equal && !(new->value = ft_strndup(equal + 1, ft_strlen(equal + 1))))
        return (perror(MINI), free(new->key), free(new), NULL);
	new->next = NULL;
	return (new);
}

int is_valid_identifier(char *s)
{
    if (*s != '_' && !ft_isalpha(*s))
        return (0);
    s++;
    while(*s && *s != '=')
    {
        if (*s != '_' && !ft_isalnum(*s))
            return (0); 
        s++;
    }
    return (1);
}
