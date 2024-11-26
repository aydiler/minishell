/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 16:58:25 by adiler            #+#    #+#             */
/*   Updated: 2024/11/26 17:54:32 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	execute_command(char **args)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
		return (perror("fork"), 1);
	if (pid == 0)
	{
		if (execve(args[0], args, NULL) == -1)
		{
			if (errno == ENOENT || errno == ENOTDIR || errno == ELOOP)
				exit(127);
			else if (errno == EACCES || errno == EISDIR)
				exit(126);
			exit(1);
		}
	}
	else
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
		{
			status = WEXITSTATUS(status);
			if (status == 127)
				ft_putstr_fd("minishell: command not found\n", 2);
			else if (status == 126)
				ft_putstr_fd("minishell: permission denied\n", 2);
			return (status);
		}
	}
	return (0);
}
