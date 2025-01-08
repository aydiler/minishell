/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_executor.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ubuntu <ubuntu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 21:59:22 by ubuntu            #+#    #+#             */
/*   Updated: 2025/01/08 21:59:25 by ubuntu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	free_pipes(int **pipes, int cmd_count)
{
	int	i;

	i = 0;
	while (i < cmd_count - 1)
	{
		free(pipes[i]);
		i++;
	}
	free(pipes);
}

void	free_executor(int **pipes, int cmd_count, int *pids)
{
	free_pipes(pipes, cmd_count);
	free(pids);
}
