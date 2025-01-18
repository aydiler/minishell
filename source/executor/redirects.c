/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirects.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 18:11:41 by adiler            #+#    #+#             */
/*   Updated: 2025/01/18 20:48:48 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

// int	create_empty_file(t_cmd cmd)
// {
// 	char *file;
// 	int fd;
	
// 	file = cmd.re->file;
// 	fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
// 	if (fd == -1)
// 	{
// 		print_error_message(file, ERR_PERMISSION);
// 		return (1);
// 	}
// 	close(fd);
// 	return (0);
// }

int	handle_infile(t_cmd cmd)
{
	int	fd;

	fd = open(cmd.re->file, O_RDONLY);
	if (fd == -1)
	{
		if (errno == ENOENT)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd.re->file, 2);
			ft_putstr_fd(": No such file or directory\n", 2);
		}
		else
			print_error_message(cmd.re->file, ERR_PERMISSION);
		return (1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

int	handle_outfile(t_cmd cmd)
{
	int	flags;
	int	fd;

	flags = O_WRONLY | O_CREAT;
	if (cmd.re->append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(cmd.re->file, flags, 0644);
	if (fd == -1)
	{
		print_error_message(cmd.re->file, ERR_PERMISSION);
		return (1);
	}
	if (cmd.re->real)
	{
		if (dup2(fd, STDOUT_FILENO) == -1)
		{
			close(fd);
			return (1);
		}
	}
	close(fd);
	return (0);
}

void	handle_redirection_execution(t_cmd cmd)
{
	int	error_check;
	
	error_check = 0;
	while (cmd.re)
	{
		if (cmd.re->type == OUT)
			error_check = handle_outfile(cmd);
		else if (cmd.re->type == IN)
			error_check = handle_infile(cmd);
		if (error_check)
			exit(1);
		cmd.re = cmd.re->next;
	}
}
