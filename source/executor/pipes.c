/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 18:08:36 by adiler            #+#    #+#             */
/*   Updated: 2024/12/21 18:08:42 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

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