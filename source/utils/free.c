/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 20:33:27 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/18 20:48:32 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

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

void	free_re(t_cmd **cmd)
{
	t_red	*temp;

	while ((*cmd)->re)
	{
		temp = (*cmd)->re->next;
		ft_memdel((void **)&(*cmd)->re->file);
		ft_memdel((void **)&((*cmd)->re));
		(*cmd)->re = temp;
	}
}

void	free_all(t_cmd **cmd)
{
	t_cmd	*temp;

	while (cmd && *cmd)
	{
		temp = (*cmd)->next;
		free_args(cmd);
		free_re(cmd);
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
