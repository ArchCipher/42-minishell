#include "minishell.h"

int type_redir(t_token_type t)
{
    return (t == REDIR_IN || t == REDIR_OUT || t == APPEND || t == HEREDOC);
}

int type_con(t_token_type t)
{
    return (t == PIPE_CHAR || t == AND || t == OR);
}

// static t_token *find_close_paren(t_token *token)
// {
//     int depth;

//     depth = 0;
//     while(token)
//     {
//         if (token->type == L_PAREN)
//             depth++;
//         else if (token->type == R_PAREN)
//             depth--;
//         if (!depth)
//             return (token);
//         token = token->next;
//     }
//      return (NULL);
// }

// static t_cmd	*slice_cmds(t_token *start, t_token *end)
// {
// 	t_list	cmd;

// 	cmd.head = NULL;
// 	while (start && start != end)
// 	{
//         if (!end && start->type == L_PAREN)
//             break ;
// 		if (start->type == WORD || type_redir(start->type))
// 		{
// 			cmd.new = build_cmd(&start);
// 			if (!cmd.new)
// 				return (free_cmds_err(cmd.head, start));
// 			lstadd_back((void **)&cmd.head, cmd.new, cmd.tail, TYPE_CMD);
// 			cmd.tail = cmd.new;
// 		}
// 		if (start && type_con(start->type))
// 		{
// 			((t_cmd *)cmd.new)->con = start->type;
// 			start = start->next;
// 		}
// 	}
// 	return (cmd.head);
// }

// t_cmd *build_ast(t_token *tokens)
// {
//     t_cmd *cmds;

//     cmds = parse_cmds(&tokens);
//     if (cmds)
//         free_tokens(tokens, false, NULL);
//     return (cmds);
// }

// t_cmd   *parse_cmds(t_token **tok)
// {
//     t_list  cmd;
//     void    *last;

//     cmd.head = NULL;
//     while(*tok && (*tok)->type != R_PAREN)
//     {
//         cmd.new = create_cmd(0);
//         if (!cmd.new)
//             return (free_cmds_err(cmd.head, tok));
//         if(*tok && type_redir((*tok)->type) && !build_redir(tok, (void **)&((t_cmd *)cmd.new)->redirs, &last))
// 			return (free_cmds_err(cmd.head, tok));
//         if ((*tok)->type == L_PAREN && !parse_subshell(tok, cmd.new))
//             return (free_cmds_err(cmd.head, tok));
//         else if (!build_cmd(tok, cmd.new, &last))
//             return (free_cmds_err(cmd.head, tok));
//         if (!cmd.new)
//             return (free_cmds_err(cmd.head, tok));
//         if(*tok && type_redir((*tok)->type) && !build_redir(tok, (void **)&((t_cmd *)cmd.new)->redirs, &last))
// 			return (free_cmds_err(cmd.head, tok));
// 		lstadd_back((void **)&cmd.head, cmd.new, cmd.tail, TYPE_CMD);
// 		cmd.tail = cmd.new;
//         if (type_connector((*tok)->type))
//         {
//             ((t_cmd *)cmd.new)->con = (*tok)->type;
//             *tok = (*tok)->next;
//         }
//     }
//     return (cmd.head);
// }

// // doesn't add redirs before and after

// static int  parse_subshell(t_token **tok, t_cmd *cmd)
// {
//     void    *last;

//     *tok = (*tok)->next;
//     cmd->sub = parse_cmds(tok);
//     if (!cmd->sub || !*tok || (*tok)->type != R_PAREN)
//         return (0);
//     *tok = (*tok)->next;
//     return (1);
// }

// /*
// DESCRIPTION:
// 	Builds an entire command struct with arguments and redirections.
// 	Word tokens following redir are added as redir file or delimeter(heredoc).
// 	Other word tokens are added to args.
// 	Old word tokens are made null, to avoid double free on error.
// 	Returns the newly built command struct on success, NULL on error.
// */

// static int  build_cmd(t_token **cur, t_cmd *cmd, void **last)
// {
// 	ssize_t	i;

// 	cmd->args = malloc ((count_args(*cur) + 1) * sizeof(char *));
// 	if (!cmd->args)
// 		return (perror(MINI), 0);
// 	i = 0;
// 	while (*cur && !type_con((*cur)->type) && (*cur)->type != R_PAREN)
// 	{
// 		if ((*cur)->type == WORD)
// 		{
// 			cmd->args[i++] = (*cur)->token;
// 			(*cur)->token = NULL;
// 			*cur = (*cur)->next;
// 		}
// 		else if (type_redir((*cur)->type) && !build_redir(cur, (void **)&cmd->redirs, last))
// 			return (cmd->args[i] = NULL, 0);
// 	}
// 	cmd->args[i] = NULL;
// 	return (1);
// }

// /*
// DESCRIPTION:
// 	Counts words until it reaches end of cmd / pipe character.
// 	Returns the number of words found.
// 	Although only defensive, returns -1 on leaks in the parser.
// */

// static size_t   count_args(t_token *tok)
// {
// 	ssize_t	word_count;

// 	word_count = 0;
// 	while (tok && !type_con(tok->type) && tok->type != R_PAREN)
// 	{
// 		if (tok->type == WORD)
// 			word_count++;
// 		else if (type_redir(tok->type))
// 			tok = tok->next;
// 		tok = tok->next;
// 	}
// 	return (word_count);
// }

// /*
// DESCRIPTION:
// 	Mallocates the cmd struct and char **args to fit given word_count words.
// 	Returns the newly malloced cmd struct.
// */

// static t_cmd	*create_cmd(size_t word_count)
// {
// 	t_cmd	*new;

// 	new = malloc(sizeof(t_cmd));
// 	if (!new)
// 		return (perror(MINI), NULL);
// 	if (word_count)
// 	{
// 		new->args = (char **)malloc((word_count + 1) * sizeof(char *));
// 		if (!new->args)
// 			return (perror(MINI), free(new), NULL);
// 	}
// 	else
// 		new->args = NULL;
// 	new->redirs = NULL;
// 	new->next = NULL;
// 	new->exec.builtin = -1;
// 	new->exec.pid = -1;
// 	new->con = NONE;
// 	new->sub = NULL;
// 	return (new);
// }
// /*
// DESCRIPTION:
// 	Builds redirect struct redirection token and following word token.
// 	Word token following redirection token is treated as delimeter for heredoc,
// 	and file for others.
// 	Old word tokens are made null, to avoid double free on error.
// */

// static int	build_redir(t_token **current, void **head, void **last)
// {
// 	t_redir	*new;

// 	if (*current && !(*current)->next)
// 		return (perr_msg("build_redir", "parsing error", NULL, false), 0);
// 	while (*current && (*current)->next && (*current)->type != PIPE_CHAR
// 		&& (*current)->type != WORD)
// 	{
// 		new = malloc(sizeof(t_redir));
// 		if (!new)
// 			return (perror(MINI), 0);
// 		new->file = (*current)->next->token;
// 		(*current)->next->token = NULL;
// 		new->flag = (*current)->type;
// 		new->fd = -1;
// 		new->quoted = (*current)->next->quoted;
// 		new->next = NULL;
// 		lstadd_back(head, (void *)new, *last, TYPE_REDIR);
// 		*last = new;
// 		*current = (*current)->next->next;
// 	}
// 	return (1);
// }
