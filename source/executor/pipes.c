/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 18:08:36 by adiler            #+#    #+#             */
/*   Updated: 2025/01/21 19:15:29 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	setup_pipe_fds(t_cmd *cmd, int **pipes, int cmd_index, int cmd_count)
{
	int	i;

	(void)cmd;
	i = 0;
	if (cmd_index > 0)
	{
		if (dup2(pipes[cmd_index - 1][READ_END], STDIN_FILENO) == -1)
			exit(1);
	}
	if (cmd_index < cmd_count - 1)
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

int	create_pipes(int **pipes, int cmd_count)
{
	int	i;

	i = 0;
	while (i < cmd_count - 1)
	{
		pipes[i] = malloc(sizeof(int) * 2);
		if (!pipes[i])
		{
			perror("malloc");
			free_pipes(pipes, i);
			return (1);
		}
		if (pipe(pipes[i]) == -1)
		{
			perror("pipe");
			free_pipes(pipes, i);
			return (1);
		}
		i++;
	}
	return (0);
}

void	close_pipes(int **pipes, int cmd_count)
{
	int	i;

	i = 0;
	while (i < cmd_count - 1)
	{
		close(pipes[i][READ_END]);
		close(pipes[i][WRITE_END]);
		i++;
	}
}

int	**create_pipe_array(int cmd_count)
{
	int	**pipes;

	pipes = malloc(sizeof(int *) * (cmd_count - 1));
	if (!pipes)
	{
		perror("malloc");
		return (NULL);
	}
	if (create_pipes(pipes, cmd_count))
		return (NULL);
	return (pipes);
}

int	initialize_pipeline(t_cmd *cmd, int ***pipes, int **pids)
{
	int	cmd_count;

	cmd_count = count_pipes(cmd);
	*pipes = create_pipe_array(cmd_count);
	if (!*pipes)
		return (-1);
	*pids = malloc(sizeof(int) * cmd_count);
	if (!*pids)
	{
		free_pipes(*pipes, cmd_count);
		return (-1);
	}
	return (cmd_count);
}
