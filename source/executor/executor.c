/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 21:41:47 by ubuntu            #+#    #+#             */
/*   Updated: 2025/01/09 17:08:22 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	process_child_status(int status, int *all_signaled,
		int *last_signal, int *first)
{
	int	last_status;
	int	termsig;

	last_status = 0;
	if (WIFEXITED(status))
	{
		*all_signaled = 0;
		last_status = WEXITSTATUS(status);
		//printf("exit status in wait for children: %d\n", last_status);
	}
	else if (WIFSIGNALED(status))
	{
		termsig = WTERMSIG(status);
		last_status = 128 + termsig;
		if (*first)
		{
			*last_signal = termsig;
			*first = 0;
		}
		else if (termsig != *last_signal)
			*all_signaled = 0;
	}
	return (last_status);
}

static int	wait_for_children(int *pids, int cmd_count)
{
	int	status;
	int	all_signaled;
	int	last_signal;
	int	first;
	int	i;

	all_signaled = 1;
	last_signal = 0;
	first = 1;
	i = 0;
	while (i < cmd_count)
	{
		waitpid(pids[i], &status, 0);
		status = process_child_status(status, &all_signaled, &last_signal,
				&first);
		i++;
	}
	if (all_signaled && last_signal == SIGQUIT)
		ft_putstr_fd("Quit (core dumped)\n", STDERR_FILENO);
	return (status);
}

void	execute_program(t_cmd cmd, char ***envp)
{
	char	*cmd_path;

	if (is_child_builtin(cmd.args))
		execute_child_builtin(cmd.args, envp);
	cmd_path = find_command_in_path(cmd.args[0]);
	if (!cmd_path)
	{
		print_error_message(cmd.args[0], ERR_NOT_FOUND);
		exit(127);
	}
	if (execve(cmd_path, cmd.args, *envp) == -1)
	{
		free(cmd_path);
		if (errno == EACCES)
		{
			print_error_message(cmd.args[0], ERR_PERMISSION);
			exit(126);
		}
		print_error_message(cmd.args[0], 0);
		exit(127);
	}
}

static int	spawn_processes(t_cmd *cmd, int **pipes, int *pids, char ***envp)
{
	int	i;
	int	cmd_count;

	i = 0;
	cmd_count = count_pipes(cmd);
	while (cmd)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			return (0);
		}
		if (pids[i] == 0)
		{
			setup_child_signals();
			handle_redirection_execution(*cmd);
			setup_pipe_fds(cmd, pipes, i, cmd_count);
			execute_program(*cmd, envp);
			exit(1);
		}
		cmd = cmd->next;
		i++;
	}
	return (1);
}

int	execute_pipeline(t_cmd *cmd, char ***envp)
{
	int	**pipes;
	int	*pids;
	int	cmd_count;
	int	last_status;

	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	if (!cmd->next && is_parent_builtin(cmd->args))
		return (execute_parent_builtin(cmd, envp));
	cmd_count = initialize_pipeline(cmd, &pipes, &pids);
	if (cmd_count == -1)
		return (1);
	g_child_running = 1;
	if (!spawn_processes(cmd, pipes, pids, envp))
	{
		free_executor(pipes, cmd_count, pids);
		return (1);
	}
	close_pipes(pipes, cmd_count);
	last_status = wait_for_children(pids, cmd_count);
	g_child_running = 0;
	setup_parent_signals();
	free_executor(pipes, cmd_count, pids);
	return (last_status);
}
