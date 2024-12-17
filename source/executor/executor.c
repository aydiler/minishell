/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 16:58:25 by adiler            #+#    #+#             */
/*   Updated: 2024/12/17 18:15:52 by adiler           ###   ########.fr       */
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
    if (cmd_index > 0)
    {
        if (!cmd->input_file)
        {
            if (dup2(pipes[cmd_index - 1][0], STDIN_FILENO) == -1)
                exit(1);
        }
    }

    if (cmd_index < cmd_count - 1)
    {
        if (!cmd->output_file)
        {
            if (dup2(pipes[cmd_index][1], STDOUT_FILENO) == -1)
                exit(1);
        }
    }
}

static int handle_child_process(t_cmd cmd, int **pipes, int cmd_index, int cmd_count, char **env)
{	
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);

	handle_redirection_execution(cmd);
	setup_pipe_fds(&cmd, pipes, cmd_index, cmd_count);
	execute_program(cmd, env);

	return (0);
}

static int handle_parent_process(pid_t pid, void (*signal_handler)(int))
{
	int status;

	signal(SIGINT, SIG_IGN);
	waitpid(pid, &status, 0);
	signal(SIGINT, signal_handler);
	if (WIFEXITED(status))
		return WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		return 128 + WTERMSIG(status);
	return 1;
}

int execute_command(t_cmd cmd, int **pipes, int cmd_index, int cmd_count, char **envp, void (*signal_handler)(int))
{
	pid_t pid;
	int status;
	int original_stdout = -1;
	int original_stdin = -1;

	if (!cmd.args || !cmd.args[0])
		return 0;
	set_original_fds(cmd, &original_stdout, &original_stdin);
	pid = fork();
	if (pid == -1) {
		perror("fork");
		return 1;
	}
	if (pid == 0)
		return (handle_child_process(cmd, pipes, cmd_index, cmd_count, envp));
    else
    {
        if (cmd_index > 0)
            close(pipes[cmd_index - 1][0]);
        if (cmd_index < cmd_count - 1)
            close(pipes[cmd_index][1]);
        status = handle_parent_process(pid, signal_handler);
        reset_fds(cmd, &original_stdout, &original_stdin);
        return status;
    }
}

int	count_pipes(t_cmd *cmd)
{
	int count;

	count = 0;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}

void	free_pipes(int **pipes, int cmd_count)
{
	int i;

	i = 0;
	while (i < cmd_count - 1)
	{
		free(pipes[i]);
		i++;
	}
	free(pipes);
}

int	**create_pipe_array(int cmd_count)
{
	int **pipes;

	pipes = malloc(sizeof(int *) * (cmd_count - 1));
	if (!pipes)
	{
		perror("malloc");
		return NULL;
	}
	if (create_pipes(pipes, cmd_count))
		return NULL;
	return pipes;
}

int	create_pipes(int **pipes, int cmd_count)
{
	int i;

	i = 0;
	while (i < cmd_count - 1)
	{
		pipes[i] = malloc(sizeof(int) * 2);
		if (!pipes[i])
		{
			perror("malloc");
			free_pipes(pipes, i);
			return 1;
		}
		if (pipe(pipes[i]) == -1)
		{
			perror("pipe");
			free_pipes(pipes, i);
			return 1;
		}
		i++;
	}
	return 0;
}



int execute_pipeline(t_cmd *cmd, char **envp, void (*signal_handler)(int))
{
    int		cmd_count;
    int		**pipes;
	t_cmd	*current;
	int		status;
	int		i;

	current = cmd;
	cmd_count = count_pipes(cmd);
	pipes = create_pipe_array(cmd_count);
	if (!pipes)
		return 1;
	if (cmd_count == 1)
		return execute_command(*cmd, pipes, 0, 1, envp, signal_handler);
	i = 0;
    while (current && i < cmd_count)
    {
        status = execute_command(*current, pipes, i, cmd_count, envp, signal_handler);
        current = current->next;
        i++;
    }
    free_pipes(pipes, cmd_count - 1);
    return status;
}