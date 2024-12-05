/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 20:33:27 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/30 18:23:49 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	free_args(t_cmd *cmd)
{
	int	i;

	i = 0;
	if (!cmd || !cmd->args)
		return ;
	while (cmd->args[i])
	{
		free(cmd->args[i]);
		i++;
	}
	free(cmd->args);
}

void	free_files_to_create(t_cmd *cmd)
{
	int	i;

	i = 0;
	if (!cmd || !cmd->files_to_create)
		return ;
	while (cmd->files_to_create[i])
	{
		free(cmd->files_to_create[i]);
		i++;
	}
	free(cmd->files_to_create);
}

void	free_other(t_cmd *cmd)
{
	if (cmd->input_file)
		free(cmd->input_file);
	else if (cmd->input_file)
		free(cmd->input_file);
}

void	free_all(t_cmd *cmd)
{
	t_cmd	*temp;

	if (!cmd)
		return ;
	while (cmd)
	{
		temp = cmd->next;
		free_args(cmd);
		free_files_to_create(cmd);
		free_other(cmd);
		free(cmd);
		cmd = temp;
	}
	cmd = NULL;
}
