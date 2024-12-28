/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built-ins_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 16:18:06 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/28 11:33:47 by maahoff          ###   ########.fr       */
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

int	ft_free_arr(char **new_envp, int j)
{
	while (--j >= 0)
		free(new_envp[j]);
	free(new_envp);
	return (ERR_NOMEM);
}
