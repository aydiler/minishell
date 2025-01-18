/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirects.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 18:11:41 by adiler            #+#    #+#             */
/*   Updated: 2025/01/18 19:42:33 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	create_empty_files(t_cmd cmd)
{
	char	**files;
	int		fd;

	files = cmd.files_to_create;
	if (!files)
		return (0);
	while (*files)
	{
		fd = open(*files, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
		{
			print_error_message(*files, ERR_PERMISSION);
			return (1);
		}
		close(fd);
		files++;
	}
	return (0);
}

int	handle_infile(t_cmd cmd)
{
	int	i;
	int	fd;

	i = 0;
	while (cmd.input_files && cmd.input_files[i])
	{
		fd = open(cmd.input_files[i], O_RDONLY);
		if (fd == -1)
		{
			if (errno == ENOENT)
			{
				ft_putstr_fd("minishell: ", 2);
				ft_putstr_fd(cmd.input_files[i], 2);
				ft_putstr_fd(": No such file or directory\n", 2);
			}
			else
				print_error_message(cmd.input_files[i], ERR_PERMISSION);
			return (1);
		}
		if (dup2(fd, STDIN_FILENO) == -1)
		{
			perror("dup2");
			close(fd);
			return (1);
		}
		close(fd);
		i++;
	}
	return (0);
}

int	handle_outfile(t_cmd cmd)
{
	int	flags;
	int	fd;

	flags = O_WRONLY | O_CREAT;
	if (cmd.append_outfile)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(cmd.output_file, flags, 0644);
	if (fd == -1)
	{
		print_error_message(cmd.output_file, ERR_PERMISSION);
		return (1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

void	handle_redirection_execution(t_cmd cmd)
{
	int	error_check;

	error_check = 0;
	// if (create_empty_files(cmd))
	// 	exit(1);
	while (cmd->re->file)
	{
		if (cmd->re->type == OUT)
			error_check = handle_outfile(cmd, cmd->re->id);
		else if (cmd->re->type == IN)
			error_check = handle_infile(cmd, cmd->re->id);
		if (error_check)
			exit(1);
		cmd->re = cmd->re->next;
	}
	// if (cmd.input_file)
	// {
	// 	if (handle_infile(cmd))
	// 		exit(1);
	// }
	// if (cmd.output_file)
	// {
	// 	if (handle_outfile(cmd))
	// 		exit(1);
	// }
}
