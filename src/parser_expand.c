#include "minishell.h"

static void	append_char_to_wrod(char *s, t_token_type *flag, char *end, t_string *str);

/*
DESCRIPTION:
	Handles the word token and returns 1 if successful, 0 on error.
*/

int	expand_word_token(t_token *tok, char *end, t_shell *shell)
{
	t_token_type	flag;
	t_string		str;

	str.s = malloc(tok->len + 1);
	if (!str.s)
		return (perror(MINI), 1);
	str.cap = tok->len;
	str.len = 0;
	flag = WORD;
	while (tok->token < end)
	{
		append_char_to_wrod(tok->token, &flag, end, &str);
		if (dollar_expandable(tok->token, end) && flag != SQUOTE)
		{
			tok->token++;
			if (append_var(&str, get_var(&tok->token, end, shell), end
					- tok->token))
				return (free(str.s), 1);
		}
		else
			tok->token++;
	}
	str.s[str.len] = 0;
	tok->token = str.s;
	return (0);
}

/*
DESCRIPTION:
	Appends a character to the string unless it is a quote or an expandable
	dollar sign. In case of quotes, it updates the flag to the appropriate type.
*/

static void	append_char_to_wrod(char *s, t_token_type *flag, char *end, t_string *str)
{
	t_token_type	old_flag;

	old_flag = *flag;
	*flag = update_quote_flag(s, *flag);
	if (*flag == old_flag && !(*flag != SQUOTE && dollar_expandable(s, end)))
		str->s[(str->len)++] = *s;
}
