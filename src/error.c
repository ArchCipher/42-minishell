#include "minishell.h"

void	perr_msg(const char *s1, const char *s2, const char *s3, bool backtick)
{
	ft_dprintf(FD_ERR, "%s: %s", MINI, s1);
	if (s2 && !backtick)
		ft_dprintf(FD_ERR, ": %s", s2);
	else if (s2 && backtick)
		ft_dprintf(FD_ERR, ": `%s'", s2);
	if (s3)
		ft_dprintf(FD_ERR, ": %s", s3);
	write(FD_ERR, "\n", 1);
}

/*
DESCRIPTION:
	Frees the command list and their content and tokens and returns NULL.
*/

t_cmd	*free_cmds_err(t_cmd *cmds, t_token *tokens)
{
	free_cmds(cmds);
	free_tokens(tokens, true, NULL);
	return (NULL);
}
