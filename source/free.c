/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 20:33:27 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/27 20:47:13 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	free_args(t_cmd **cmd)
{
	int	i;

	i = 0;
	if (!(*cmd) || !(*cmd)->args)
		return ;
	while ((*cmd)->args[i])
	{
		free((*cmd)->args[i]);
		i++;
	}
}

void	free_all(t_cmd **cmd)
{
	t_cmd	*temp;

	if (!cmd || !(*cmd))
		return ;
	while (*cmd)
	{
		temp = (*cmd)->next;
		free_args(cmd);
		free(*cmd);
		*cmd = temp;
	}
	*cmd = NULL;
}
