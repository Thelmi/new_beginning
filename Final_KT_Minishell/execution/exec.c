/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thelmy <thelmy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 11:27:11 by mrhelmy           #+#    #+#             */
/*   Updated: 2024/09/27 17:36:17 by thelmy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_exec(t_main main, t_env **envir,
					t_export **exp, int *last_exit_status)
{
	char	*path;

	signal(SIGQUIT, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	if (ft_strcmp((main.ecmd)->argv[0], "cat")
		&& (main.ecmd)->argv[1] == NULL && *(main.has_heredoc))
		cat_with_heredoc(main, envir, exp);
	remove_quotes(main.cmd);
	if (env_path(*envir, last_exit_status)
		|| ft_strchr((main.ecmd)->argv[0], '/'))
	{
		if (ft_strcmp((main.ecmd)->argv[0], "cat"))
			*(main.stop_cat) = 0;
		if (execve((main.ecmd)->argv[0], (main.ecmd)->argv, (*envir)->ev) == -1)
		{
			path = find_path((main.ecmd)->argv[0], (*envir)->ev);
			if (path)
				execve(path, (main.ecmd)->argv, (*envir)->ev);
			free(path);
		}
	}
	free_failed_exec(main, envir, exp, last_exit_status);
}

void	exec_signal_handler(t_main main, int *last_exit_status)
{
	int	sig;

	if (WIFSIGNALED(main.status))
	{
		sig = WTERMSIG(main.status);
		if (sig == SIGINT)
		{
			write(1, "^C\n", 3);
			*last_exit_status = 130;
		}
		else if (sig == SIGQUIT)
		{
			write(1, "^\\Quit: 3\n", 10);
			*last_exit_status = 131;
		}
	}
	else
	{
		*last_exit_status = WEXITSTATUS(main.status);
	}
}

void	reset_value(t_main main, int *last_exit_status)
{
	wait(&main.status);
	exec_signal_handler(main, last_exit_status);
	close (main.saved_stdout);
	close (main.saved_stdin);
	if (main.input)
	{
		free (main.input);
	}
	if (ft_strcmp((main.ecmd)->argv[0], "cat")
		&& (main.ecmd)->argv[1] == NULL && *(main.has_heredoc))
	{
		*(main.has_heredoc) = 0;
	}
	if (!ft_strcmp((main.ecmd)->argv[0], "cat")
		|| (ft_strcmp((main.ecmd)->argv[0], "cat") && (main.ecmd)->argv[1]))
	{
		*(main.stop_cat) = 0;
	}
	main.input = NULL;
}

int	valid_args(t_main main, int *last_exit_status)
{
	if (!((main.ecmd)->argv[0]))
	{
		*last_exit_status = 1;
		if (main.input)
			free (main.input);
		main.input = NULL;
		return (0);
	}
	if (!ft_strcmp((main.ecmd)->argv[0], "cat")
		|| (ft_strcmp((main.ecmd)->argv[0], "cat") && (main.ecmd)->argv[1]))
	{
		*(main.stop_cat) = 1;
	}
	return (1);
}

void	exec(t_main main, t_env **envir, t_export **exp, int *last_exit_status)
{
	t_norm		x;
	struct cmd	*cmd;

	cmd = main.cmd;
	close (main.saved_stdout);
	close (main.saved_stdin);
	main.ecmd = (struct execcmd *)cmd;
	if (!valid_args(main, last_exit_status))
		return ;
	if (is_builtin((main.ecmd)->argv[0]) == true)
	{
		if (main.input)
			free (main.input);
		main.input = NULL;
		x.var1 = (void **)envir;
		x.var2 = (void **)((main.ecmd)->argv);
		*envir = execute_builtin(x, (main.ecmd)->echar, last_exit_status, exp);
		return ;
	}
	if (fork() == 0)
		exec_exec(main, envir, exp, last_exit_status);
	else
		return (reset_value(main, last_exit_status));
}

// if (!(main.ecmd)->argv[0])
// {
//   if (main.input)
//     free (main.input);
//   main.input = NULL;
//   return ;
// }
// if (!ft_strcmp((main.ecmd)->argv[0], "cat") 
//|| (ft_strcmp((main.ecmd)->argv[0], "cat") && (main.ecmd)->argv[1]))
// {
//   *(main.stop_cat) = 1;
// }
// if ((main.ecmd)->argv[0] == NULL)
// {
// 	*last_exit_status = 1;
//   if (main.input)
//     free(main.input);
//   main.input = NULL;
// 	return ; 
// }
