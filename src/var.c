#include "minishell.h"

static char			*status_itoa(int n, char *num);
static const char	*expand_var(char **token, char *end, t_env *env);
static int			valid_substitution(char *token, size_t len);
static size_t		ft_numlen(int n, int base);

/*
DESCRIPTION:
	Wrapper function to expand the variable in the token.
	Returns the status variable or the expanded variable on success,
	ERR_PTR on invalid substitution.

NOTE:
	As NULL is a valid expansion, ERR_PTR is (char *)-1, used as a sentinal value for invalid substitution.
	Must not be dereferenced, but only compared to detect invalid substitution.
*/

const char *get_var(char **token, char *end, t_shell *shell)
{
	char	exit_code[4];

	if (**token != '?')
		return(expand_var(token, end, shell->env));
	(*token)++;
	return (status_itoa(shell->status, exit_code));
}

/*
DESCRIPTION:
	Converts and returns the status variable as a string within the range of 0-255.
*/

static char	*status_itoa(int n, char *num)
{
	size_t	len;

	n = n % EXIT_STATUS_MOD;
	len = ft_numlen(n, 10);
	num[len] = '\0';
	if (n == 0)
		num[0] = '0';
	while (len-- > 0 && n)
	{
		num[len] = (n % 10) + '0';
		n /= 10;
	}
	return (num);
}

/*
DESCRIPTION:
	Expands the variable in the token.
	Returns the expanded variable on success, ERR_PTR on invalid substitution.

NOTE:
	As NULL is a valid expansion, ERR_PTR is (char *)-1, used as a sentinal
	value for invalid substitution and malloc failure.
	Must not be dereferenced, but only compared to detect invalid substitution.
*/

static const char	*expand_var(char **token, char *end, t_env *env)
{
	const char	*env_var;
	char		*start;
	char		*tmp;

	start = *token;
	while (*token < end && (**token == '_' || ft_isalnum(**token)))
		(*token)++;
	if (*token == start && **token == '{' && valid_substitution(*token, end
			- *token))
	{
		start = *token + 1;
		*token = ft_memchr(*token, '}', end - *token);
	}
	else if (**token == '{')
		return (ERR_PTR);
	tmp = ft_strndup(start, *token - start);
	if (!tmp)
		return (perror(MINI), ERR_PTR);
	env_var = ft_getenv(env, tmp);
	free(tmp);
	if (**token == '}')
		(*token)++;
	return (env_var);
}

/*
DESCRIPTION:
	Checks if the substitution is valid.
	Returns 1 if valid, 0 if invalid.
*/

static int	valid_substitution(char *token, size_t len)
{
	size_t	i;

	if (token[len - 1] == '\'' || token[len - 1] == '\"')
		len--;
	if (len == 1)
		return (ft_dprintf(STDERR_FILENO, "%s: %s\n", MINI, E_ENV), 0);
	i = 1;
	while (i < len && (token[i] == '_' || ft_isalnum(token[i])))
		i++;
	if (token[i] != '}')
	{
		ft_dprintf(STDERR_FILENO, "%s: $", MINI);
		write(STDERR_FILENO, token, len);
		ft_dprintf(STDERR_FILENO, ": %s\n", E_ENV);
		return (0);
	}
	return (1);
}

/*
DESCRIPTION:
	Calculates the length of the number in the given base.
	Returns the length of the number.
*/

static size_t	ft_numlen(int n, int base)
{
	size_t	count;

	count = 0;
	if (n <= 0)
		count++;
	while (n)
	{
		count++;
		n /= base;
	}
	return (count);
}
