/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 16:58:25 by adiler            #+#    #+#             */
/*   Updated: 2024/12/24 17:42:46 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void execute_program(t_cmd cmd, char **envp)
{
	char *cmd_path;
	
	if (!cmd.args || !cmd.args[0])
        exit(0); 
	if (is_child_builtin(cmd.args))
		execute_child_builtin(cmd.args, envp);
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
	if (cmd_index > 0 && !cmd->input_file)
	{
			if (dup2(pipes[cmd_index - 1][READ_END], STDIN_FILENO) == -1)
				exit(1);
	}
	if (cmd_index < cmd_count - 1 && !cmd->output_file)
	{
			if (dup2(pipes[cmd_index][WRITE_END], STDOUT_FILENO) == -1)
				exit(1);
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

static int initialize_pipeline(t_cmd *cmd, int ***pipes, int **pids)
{
	int cmd_count = count_pipes(cmd);
	
	*pipes = create_pipe_array(cmd_count);
	if (!*pipes)
		return -1;
		
	*pids = malloc(sizeof(int) * cmd_count);
	if (!*pids)
	{
		free_pipes(*pipes, cmd_count);
		return -1;
	}
	return cmd_count;
}

static int spawn_processes(t_cmd *cmd, int **pipes, int *pids, 
						 int cmd_count, char **envp)
{
	int i = 0;
	t_cmd *current = cmd;

	while (current)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			return 0;
		}
		if (pids[i] == 0)
		{
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			handle_redirection_execution(*current);
			setup_pipe_fds(current, pipes, i, cmd_count);
			execute_program(*current, envp);
			exit(1);
		}
		current = current->next;
		i++;
	}
	return 1;
}

static void close_pipes(int **pipes, int cmd_count)
{
	int i = 0;

	while (i < cmd_count - 1)
	{
		close(pipes[i][READ_END]);
		close(pipes[i][WRITE_END]);
		i++;
	}
}

static int wait_for_children(int *pids, int cmd_count)
{
	int i = 0;
	int status;
	int last_status = 0;

	while (i < cmd_count)
	{
		waitpid(pids[i], &status, 0);
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			last_status = 128 + WTERMSIG(status);
		i++;
	}
	return last_status;
}

int execute_pipeline(t_cmd *cmd, char **envp, void (*signal_handler)(int))
{
	int **pipes;
	int *pids;
	int cmd_count;
	int last_status;

	if(!cmd || !cmd->args || !cmd->args[0])
		return 0;
	if (!cmd->next && is_parent_builtin(cmd->args))
		return execute_parent_builtin(cmd, envp);
	cmd_count = initialize_pipeline(cmd, &pipes, &pids);
	if (cmd_count == -1)
		return 1;
	if (!spawn_processes(cmd, pipes, pids, cmd_count, envp))
	{
		free_pipes(pipes, cmd_count);
		free(pids);
		return 1;
	}
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	close_pipes(pipes, cmd_count);
	last_status = wait_for_children(pids, cmd_count);
	free_pipes(pipes, cmd_count);
	free(pids);
	return last_status;
}
