/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 20:33:27 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/17 15:16:43 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	free_args(t_cmd **cmd)
{
	int	i;

	i = 0;
	if (!cmd || !(*cmd) || !(*cmd)->args)
		return ;
	while ((*cmd)->args[i])
	{
		ft_memdel((void **)&((*cmd)->args[i]));
		i++;
	}
	ft_memdel((void **)&((*cmd)->args));
}

void	free_files_to_create(t_cmd **cmd)
{
	int	i;

	i = 0;
	if (!cmd || !(*cmd)|| !(*cmd)->files_to_create)
		return ;
	while ((*cmd)->files_to_create[i])
	{
		ft_memdel((void **)&((*cmd)->files_to_create[i]));
		i++;
	}
	ft_memdel((void **)&((*cmd)->files_to_create));
}

void	free_other(t_cmd **cmd)
{
	if ((*cmd)->input_file)
		ft_memdel((void **)&((*cmd)->input_file));
	if ((*cmd)->output_file)
		ft_memdel((void **)&((*cmd)->output_file));
}

void	free_all(t_cmd **cmd)
{
	t_cmd	*temp;

	while (cmd && *cmd)
	{
		temp = (*cmd)->next;
		free_args(cmd);
		free_files_to_create(cmd);
		free_other(cmd);
		ft_memdel((void **)&((*cmd)));
		*cmd = temp;
	}
	*cmd = NULL;
}

void	ft_free_split(char **str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		ft_memdel((void **)&(str[i]));
		i++;
	}
	ft_memdel((void **)&(str));
}
