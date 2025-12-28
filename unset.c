# include "minishell.h"

/*
unset	0
*/

int exec_unset(t_shell *shell, char **args)
{
    t_env   *prev;
    t_env   *env;

    if (!*args)
        return (0);
    while(*args)
    {
        if (!is_valid_identifier(*args))
            return (ft_dprintf(STDERR_FILENO, "%s: unset: `%s': %s\n",MINI, *args, E_EXPORT), 1);
        env = env_lookup_prev(shell->env, &prev, *args);
        if (env)
        {
            if (prev)
                prev->next = env->next;
            else
                shell->env = env->next;
            if (env == shell->env_tail)
                shell->env_tail = prev;
            free(env->key);
            if (env->value)
                free(env->value);
            free(env);
        }
        args++;
    }
    return (0);
}

t_env *env_lookup_prev(t_env *env, t_env **prev, const char *arg)
{
    char    *equal;
    size_t  key_len;

    *prev = NULL;
    key_len = ft_strlen(arg);
    equal = ft_memchr(arg, '=', key_len);
    if (equal)
        key_len = equal - arg;
    while (env)
    {
        if (ft_strlen(env->key) == key_len && !ft_strncmp(env->key, arg, key_len))
            return (env);
        *prev = env;
        env = env->next;
    }
    return (NULL);
}
