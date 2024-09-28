/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thelmy <thelmy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 11:19:22 by mrhelmy           #+#    #+#             */
/*   Updated: 2024/09/28 17:17:02 by thelmy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_main	parsecmd(char *s, t_env *envir, int *last_exit_status)
{
	char		*es;
	struct cmd	*cmd;
	t_main		main;

	cmd = NULL;
	main.heredoc = NULL;
	main.cmd = NULL;
	if (check_quotes(s))
		return (main);
	es = s + ft_strlen(s);
	main.command = s;
	cmd = parsepipe(&s, es, &(main.heredoc), last_exit_status);
	peek(&s, es, "");
	if (cmd && s != es)
	{
		free_wrong_parsing(s, cmd, main, last_exit_status);
		return (main);
	}
	nulterminate(cmd, envir, last_exit_status);
	main.cmd = cmd;
	main.main_cmd = cmd;
	return (main);
}

t_cmd	*parseredirs(t_norm x, t_cmd *cmd, char *es, int *last_exit_status)
{
	int		tok;
	char	*q;
	char	*eq;
	t_norm	y;

	while (peek((char **)(x.var1), es, "<>"))
	{
		tok = gettoken((char **)(x.var1), es, 0, 0);
		if (gettoken((char **)(x.var1), es, &q, &eq) != 'a'
			&& print_token_error(cmd, last_exit_status))
			return (NULL);
		y.var1 = (void **)q;
		y.var2 = (void **)eq;
		if (tok == '<')
			cmd = redircmd(cmd, y, O_RDONLY, 0);
		else if (tok == '>')
			cmd = redircmd(cmd, y, O_WRONLY | O_CREAT | O_TRUNC, 1);
		else if (tok == '+')
			cmd = redircmd(cmd, y, O_WRONLY | O_CREAT | O_APPEND, 1);
		else if (tok == 'h')
			redircmd_h(q, eq, (t_heredoc **)(x.var2));
	}
	return (cmd);
}

t_cmd	*parsepipe(char **ps, char *es, t_heredoc **heredoc,
		int *last_exit_status)
{
	t_cmd	*cmd;

	cmd = parseexec(ps, es, heredoc, last_exit_status);
	if (cmd && peek(ps, es, "|"))
	{
		gettoken(ps, es, 0, 0);
		cmd = pipecmd(cmd, parsepipe(ps, es, heredoc, last_exit_status));
	}
	return (cmd);
}

t_cmd	*parseexec(char **ps, char *es, t_heredoc **heredoc,
		int *last_exit_status)
{
	int		argc;
	t_cmd	*ret;
	t_norm	x;

	x.cmd = initialize_execcmd(&ret, last_exit_status);
	if (!x.cmd)
		return (NULL);
	x.var1 = (void **)ps;
	x.var2 = (void **)heredoc;
	ret = parseredirs(x, ret, es, last_exit_status);
	if (!ret)
		return (NULL);
	argc = get_argv(&x, &ret, es, last_exit_status);
	if (argc == -1)
		return (NULL);
	if (ret && x.cmd && x.cmd->argv[argc])
		x.cmd->argv[argc] = 0;
	if (ret && x.cmd && x.cmd->eargv[argc])
		x.cmd->eargv[argc] = 0;
	return (ret);
}
