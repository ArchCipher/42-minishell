#include "minishell.h"

static int	should_expand(char *s, char *end, t_token *prev,
				t_token_type flag);
static void	append_char(char *c, char *end, t_token_type *flag, t_string *str);

/*
DESCRIPTION:
	Handles the word token and returns 0 on success, 1 on error.
*/

int	process_word_token(t_token *tok, t_token *prev, t_shell *shell)
{
	t_token_type	flag;
	t_string		str;
	char			*end;

	str = alloc_tstring(tok->len);
	if (!str.s)
		return (perror(MINI), 1);
	end = tok->token + tok->len;
	flag = WORD;
	while (tok->token < end)
	{
		if (!should_expand(tok->token, end, prev, flag))
		{
			append_char(tok->token, end, &flag, &str);
			tok->token++;
		}
		else if (append_var(&str, &tok->token, end, shell))
			return (free(str.s), 1);
	}
	str.s[str.len] = 0;
	tok->token = str.s;
	return (0);
}

static int	should_expand(char *s, char *end, t_token *prev,
		t_token_type flag)
{
	if (prev && prev->type == HEREDOC)
		return (0);
	if (*s == '~' && (s + 1 == end || s[1] == '/'))
		return (1);
	if (dollar_expandable(s, end) && flag != SQUOTE)
		return (1);
	return (0);
}

/*
DESCRIPTION:
	Appends a character to the string unless it is a quote or an expandable
	dollar sign. In case of quotes, it updates the flag to the appropriate type.

	Updates quote state. Does not append the quote character itself

	skips '$' for following cases,but does not support ANSCI-C quoting or locale
	translation
	$'string'	ANSI-C quoting
	$"string"	locale translation
*/

static void	append_char(char *s, char *end, t_token_type *flag, t_string *str)
{
	t_token_type	old_flag;

	if (*s == '$' && *flag == WORD && s + 1 < end && (s[1] == '\''
			|| s[1] == '\"'))
		return ;
	old_flag = *flag;
	*flag = update_quote_flag(*s, *flag);
	if (*flag == old_flag)
		str->s[(str->len)++] = *s;
}
