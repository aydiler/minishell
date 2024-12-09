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

static int create_empty_files(t_cmd cmd)
{
	char **files = cmd.files_to_create;
	int fd;

	if (!files)
		return 0;
	while (*files)
	{
		fd = open(*files, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
		{
			print_error_message(*files, ERR_PERMISSION);
			return 1;
		}
		close(fd);
		files++;
	}
	return 0;
}

static int handle_infile(t_cmd cmd)
{
	int fd = open(cmd.input_file, O_RDONLY);
	if (fd == -1)
	{
		if (errno == ENOENT)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd.input_file, 2);
			ft_putstr_fd(": No such file or directory\n", 2);
		}
		else
			print_error_message(cmd.input_file, ERR_PERMISSION);
		return 1;
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return 1;
	}
	close(fd);
	return 0;
}

static int handle_outfile(t_cmd cmd)
{
	int flags = O_WRONLY | O_CREAT;
	if (cmd.append_outfile)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	int fd = open(cmd.output_file, flags, 0644);
	if (fd == -1) {
		print_error_message(cmd.output_file, ERR_PERMISSION);
		return 1;
	}
	if (dup2(fd, STDOUT_FILENO) == -1) {
		close(fd);
		return 1;
	}
	close(fd);
	return 0;
}

static int handle_child_process(t_cmd cmd, char **env)
{
	char *cmd_path;
	
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);

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

	cmd_path = find_command_in_path(cmd.args[0]);
	if (!cmd_path)
	{
		print_error_message(cmd.args[0], ERR_NOT_FOUND);
		exit(127);
	}
	if (execve(cmd_path, cmd.args, env) == -1)
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

int execute_command(t_cmd cmd, char **envp, void (*signal_handler)(int))
{
	pid_t pid;
	int status;
	int original_stdout = -1;
	int original_stdin = -1;
	
	if (!cmd.args || !cmd.args[0])
		return 0;
	if (cmd.output_file)
		original_stdout = dup(STDOUT_FILENO);
	if (cmd.input_file)
		original_stdin = dup(STDIN_FILENO);

	pid = fork();
	if (pid == -1) {
		perror("fork");
		return 1;
	}

	if (pid == 0)
		return (handle_child_process(cmd, envp));
	else
	{
		status = handle_parent_process(pid, signal_handler);
		if (original_stdin != -1)
		{
			dup2(original_stdin, STDIN_FILENO);
			close(original_stdin);
		}
		if (original_stdout != -1)
		{
			dup2(original_stdout, STDOUT_FILENO);
			close(original_stdout);
		}
		return status;
	}
}