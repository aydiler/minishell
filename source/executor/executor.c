/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 16:58:25 by adiler            #+#    #+#             */
/*   Updated: 2024/12/21 19:07:38 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	create_pipes(int **pipes, int cmd_count);

void	handle_redirection_execution(t_cmd cmd)
{
	if (create_empty_files(cmd))
		exit(1);
	if (cmd.input_file)
	{
		if (handle_infile(cmd))
			exit(1);
	}
	if (cmd.output_file)
	{
		if (handle_outfile(cmd))
			exit(1);
	}
}

void	execute_program(t_cmd cmd, char **envp)
{
	char *cmd_path;
	
	cmd_path = find_command_in_path(cmd.args[0]);
	if (!cmd_path)
	{
		print_error_message(cmd.args[0], ERR_NOT_FOUND);
		exit(127);
	}
	
	if (execve(cmd_path, cmd.args, envp) == -1)
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

static void setup_pipe_fds(t_cmd *cmd, int **pipes, int cmd_index, int cmd_count)
{
	int	i;

	i = 0;
	if (cmd_index > 0)
	{
		if (!cmd->input_file)
		{
			if (dup2(pipes[cmd_index - 1][READ_END], STDIN_FILENO) == -1)
				exit(1);
		}
	}
	if (cmd_index < cmd_count - 1)
	{
		if (!cmd->output_file)
		{
			if (dup2(pipes[cmd_index][WRITE_END], STDOUT_FILENO) == -1)
				exit(1);
		}
	}
	while (i < cmd_count - 1)
	{
		if (i != cmd_index - 1)
			close(pipes[i][READ_END]);
		if (i != cmd_index)
			close(pipes[i][WRITE_END]);
		i++;
	}
}

static int handle_child_process(t_cmd cmd, int **pipes, int cmd_index, int cmd_count, char **env)
{	
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);

	handle_redirection_execution(cmd);
	setup_pipe_fds(&cmd, pipes, cmd_index, cmd_count);
	execute_program(cmd, env);
	exit(1);
}


int execute_pipeline(t_cmd *cmd, char **envp, void (*signal_handler)(int))
{
	int     cmd_count;
	int     **pipes;
	t_cmd   *current;
	int     status;
	int     last_status;
	int     i;
	int     *pids;

	current = cmd;
	cmd_count = count_pipes(cmd);
	pipes = create_pipe_array(cmd_count);
	if (!pipes)
		return 1;
	pids = malloc(sizeof(int) * cmd_count);
	if (!pids)
	{
		perror("malloc");
		free_pipes(pipes, cmd_count);
		return 1;
	}
	i = 0;
	while (current)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			free_pipes(pipes, cmd_count);
			free(pids);
			return 1;
		}
		if (pids[i] == 0)
			handle_child_process(*current, pipes, i, cmd_count, envp);
		current = current->next;
		i++;
	}
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	i = 0;
	while (i < cmd_count - 1)
	{
		close(pipes[i][READ_END]);
		close(pipes[i][WRITE_END]);
		i++;
	}
	last_status = 0;
	i = 0;
	while (i < cmd_count)
	{
		waitpid(pids[i], &status, 0);
		if (WIFEXITED(status))
		{
			last_status = WEXITSTATUS(status);
			//printf("Debug: Child process %d exited with status %d\n", pids[i], last_status);
		}
		else if (WIFSIGNALED(status))
			last_status = 128 + WTERMSIG(status);
		i++;
	}
	free_pipes(pipes, cmd_count);
	free(pids);
	//printf("Debug: Pipeline executed with status %d\n", last_status);
	return last_status;
}
