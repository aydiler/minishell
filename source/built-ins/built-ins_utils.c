/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built-ins_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 16:18:06 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/29 11:39:45 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	**ft_sort_envp(char **envp)
{
	int	i;
	char *temp;

	i = 0;
	while ((envp)[i] && (envp)[i + 1])
	{
		if (ft_strcmp(envp[i], (envp)[i + 1]) > 0)
		{
			temp = (envp)[i];
			(envp)[i] = (envp)[i + 1];
			(envp)[i + 1] = temp;
			i = 0;
		}
		else
			i++;
	}
	return (envp);
}

int	ft_free_arr(char ***arr)
{
	int	i;

	i = 0;
	if (!arr || !*arr)
		return (ERR_NOMEM);
	while ((*arr)[i])
	{
		printf("i in free arr: %d\n", i);
		free((*arr)[i]);
		(*arr)[i] = NULL;
		i++;
	}
	printf("after: i:%d\n", i);
	free(*arr);
	(*arr) = NULL;
	printf("after\n");
	return (ERR_NOMEM);
}
