/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cmd_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thelmy <thelmy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 17:16:08 by thelmy            #+#    #+#             */
/*   Updated: 2024/09/29 00:10:19 by thelmy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	print_token_error(t_cmd *cmd, int *last_exit_status)
{
	write(2, "bash: syntax error near unexpected token\n", 41);
	*last_exit_status = 2;
	freecmd(cmd, 1);
	cmd = NULL;
	return (1);
}

t_execcmd	*initialize_execcmd(t_cmd **ret, int *last_exit_status)
{
	t_execcmd	*cmd;

	*ret = execcmd();
	if (!*ret)
	{
		perror("Error creating exec command");
		*last_exit_status = 1;
		return (NULL);
	}
	cmd = (t_execcmd *)*ret;
	return (cmd);
}

int	get_argv(t_norm *x, t_cmd **ret, char *es, int *last_exit_status)
{
	int		argc;
	char	*q;
	char	*eq;
	int		tok;

	argc = 0;
	while (*ret && !peek((char **)((*x).var1), es, "|"))
	{
		tok = gettoken((char **)((*x).var1), es, &q, &eq);
		if (tok == 0)
			return (argc);
		if (tok != 'a')
		{
			*last_exit_status = 2;
			write(2, "bash: syntax error near unexpected token\n", 41);
			return (freecmd(*ret, 1), -1);
		}
		(*x).cmd->argv[argc] = q;
		(*x).cmd->eargv[argc] = eq;
		argc++;
		(*x).var1 = (void **)(*x).var1;
		(*x).var2 = (void **)(*x).var2;
		*ret = parseredirs((*x), *ret, es, last_exit_status);
	}
	return (argc);
}
