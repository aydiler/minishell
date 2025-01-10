/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built-ins_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 16:18:06 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/10 19:56:58 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	**ft_sort_envp(char **envp)
{
	int		i;
	char	*temp;
	char	**sorted_envp;

	sorted_envp = ft_arrdup(envp);
	if (!sorted_envp)
		return (NULL);
	i = 0;
	while ((sorted_envp)[i] && (sorted_envp)[i + 1])
	{
		if (ft_strcmp(sorted_envp[i], (sorted_envp)[i + 1]) > 0)
		{
			temp = (sorted_envp)[i];
			(sorted_envp)[i] = (sorted_envp)[i + 1];
			(sorted_envp)[i + 1] = temp;
			i = 0;
		}
		else
			i++;
	}
	return (sorted_envp);
}

int	ft_free_arr(char ***arr)
{
	int	i;

	i = 0;
	if (!arr || !*arr)
		return (ERR_NOMEM);
	while ((*arr)[i])
	{
		free((*arr)[i]);
		(*arr)[i] = NULL;
		i++;
	}
	free(*arr);
	(*arr) = NULL;
	return (ERR_NOMEM);
}
