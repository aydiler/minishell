/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 16:58:25 by adiler            #+#    #+#             */
/*   Updated: 2024/12/06 20:44:29 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void print_error_message(char *cmd, int error_type)
{
	char *error_msg;

	if (cmd[0] == '/' || (cmd[0] == '.' && cmd[1] == '/') || 
		(cmd[0] == '.' && cmd[1] == '.' && cmd[2] == '/'))
	{
		error_msg = ft_strjoin("minishell: ", cmd);
		perror(error_msg);
		free(error_msg);
		return;
	}
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	if (error_type == ERR_NOT_FOUND)
		ft_putstr_fd(": command not found\n", 2);
	else if (error_type == ERR_PERMISSION)
		ft_putstr_fd(": Permission denied\n", 2);
}

static int handle_child_process(char **args, char **env)
{
	char *cmd_path;
	
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	cmd_path = find_command_in_path(args[0]);
	if (!cmd_path)
	{
		print_error_message(args[0], ERR_NOT_FOUND);
		exit(127);
	}
	if (execve(cmd_path, args, env) == -1)
	{
		free(cmd_path);
		if (errno == EACCES)
		{
			print_error_message(args[0], ERR_PERMISSION);
			exit(126);
		}
		print_error_message(args[0], 0);
		exit(127);
	}
	return (0);
}

static int handle_parent_process(pid_t pid, void (*signal_handler)(int), int fd)
{
	int status;

	signal(SIGINT, SIG_IGN);
	waitpid(pid, &status, 0);
	signal(SIGINT, signal_handler);
	if (fd != -1)
	{
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
	if (WIFEXITED(status))
		return WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		return 128 + WTERMSIG(status);
	return 1;
}

int	handle_outfile(t_cmd cmd, int *original_stdout)
{
	*original_stdout = dup(STDOUT_FILENO);
	int fd = open(cmd.output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1) {
		print_error_message(cmd.output_file, ERR_PERMISSION);
		return 1;
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return 0;
}

int execute_command(t_cmd cmd, char **envp, void (*signal_handler)(int))
{
	pid_t pid;
	int original_stdout = -1;
	
	if (!cmd.args || !cmd.args[0])
        return 0;
	if (cmd.output_file)
	{
		if (handle_outfile(cmd, &original_stdout))
			return 1;
	}
	pid = fork();
	if (pid == -1)
		return (perror("fork"), 1);
	if (pid == 0)
		return (handle_child_process(cmd.args, envp));
	else
		return (handle_parent_process(pid, signal_handler, original_stdout));
}
