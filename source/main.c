/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 19:01:15 by adiler            #+#    #+#             */
/*   Updated: 2025/01/23 14:51:21 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

volatile sig_atomic_t	g_child_running;
volatile sig_atomic_t	g_heredoc_signal;

int	handle_c_flag(char **argv, char **dup_envp)
{
	char	*input;
	t_cmd	*cmd;
	int		error_check;
	int		exit_status;

	input = ft_strdup(argv[2]);
	error_check = 0;
	cmd = NULL;
	exit_status = 0;
	if (input[0])
		error_check = parser(&input, &cmd, dup_envp, exit_status);
	ft_memdel((void **)&(input));
	if (error_check || !cmd)
	{
		ft_error(&cmd, error_check);
		exit_status = 1;
	}
	else if (cmd)
	{
		exit_status = execute_pipeline(cmd, &dup_envp);
		free_all(&cmd);
	}
	return (exit_status);
}

void	handle_input(char *input, char ***dup_envp, int *exit_status)
{
	t_cmd	*cmd;
	int		error_check;

	error_check = 0;
	cmd = NULL;
	if (input[0])
		error_check = parser(&input, &cmd, *dup_envp, *exit_status);
	ft_memdel((void **)&(input));
	if (error_check || !cmd)
	{
		ft_error(&cmd, error_check);
		return ;
	}
	if (cmd)
		*exit_status = execute_pipeline(cmd, dup_envp);
	free_all(&cmd);
}

void	handle_interactive_mode(char ***dup_envp, int exit_status)
{
	char	*input;

	load_history();
	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
		{
			ft_putstr_fd("exit\n", 1);
			break ;
		}
		if (*input)
		{
			add_history(input);
			save_history(input);
		}
		handle_input(input, dup_envp, &exit_status);
	}
	if (dup_envp || *dup_envp)
		ft_free_arr(dup_envp);
	rl_clear_history();
	exit(exit_status);
}

int	main(int argc, char **argv, char **envp)
{
	char	**dup_envp;
	int		exit_status;

	g_child_running = 0;
	g_heredoc_signal = 0;
	dup_envp = ft_arrdup(envp);
	exit_status = 0;
	setup_parent_signals();
	increase_shlvl(&dup_envp);
	if (argc >= 3 && ft_strcmp(argv[1], "-c") == 0)
	{
		exit_status = handle_c_flag(argv, dup_envp);
		if (dup_envp)
			ft_free_arr(&dup_envp);
		exit(exit_status);
	}
	handle_interactive_mode(&dup_envp, exit_status);
}
