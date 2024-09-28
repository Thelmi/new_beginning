/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thelmy <thelmy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/14 14:58:22 by krazikho          #+#    #+#             */
/*   Updated: 2024/09/28 20:25:09 by thelmy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	no_env(char **tmp)
{
	printf("such a dangerous behavior, keep ur children safe\n");
	if (tmp)
		free_double_pointer(tmp);
	tmp = NULL;
	return (1);
}

t_main	reset_main(t_main x, char *command)
{
	static int	start;

	start = 0;
	x = initialize_main(x);
	x.command = command;
	x.start = &start;
	x.input = NULL;
	x.cat_counter = &start;
	return (x);
}

char	**run_main(t_env **envir, t_export **exp, t_context *context,
		char *command)
{
	static char	**tmp = 0;
	t_main		x;

	if (envir && *envir)
	{
		(*envir)->ev = convert_env(envir);
		tmp = (*envir)->ev;
		update_signals();
		fill_env(envir, (*envir)->ev);
		add_history(command);
		x = parsecmd(command, *envir, &context->last_exit_status);
		x = reset_main(x, command);
		if (x.cmd)
			runcmd(x, envir, exp, &context->last_exit_status);
		if (x.cmd)
			freecmd(x.cmd, 0);
		if (envir && *envir && (*envir)->ev)
		{
			free_double_pointer((*envir)->ev);
			tmp = NULL;
		}
	}
	else if (no_env(tmp))
		tmp = NULL;
	return (tmp);
}

static void	command_loop(t_env **envir, t_export **exp, t_context *context)
{
	char	*command;
	char	**tmp;

	tmp = NULL;
	while (1)
	{
		setup_signals();
		command = readline("minishell$ ");
		if (command == NULL)
		{
			write(1, "exit\n", 5);
			break ;
		}
		if (*command)
			tmp = run_main(envir, exp, context, command);
		free(command);
	}
	if (tmp)
		no_env(tmp);
	tmp = NULL;
	free_env(*envir);
	free_export(*exp);
}

int	main(int ac, char **av, char **ev)
{
	t_env		*envir;
	t_export	*exp;
	t_context	context;

	(void)ac;
	(void)av;
	initialize_shell(ev, &envir, &exp, &context);
	command_loop(&envir, &exp, &context);
	return (context.last_exit_status);
}
