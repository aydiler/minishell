/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built-ins_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 16:18:06 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/09 20:18:34 by adiler           ###   ########.fr       */
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
		//printf("i in free arr: %d\n", i);
		free((*arr)[i]);
		(*arr)[i] = NULL;
		i++;
	}
	//printf("after: i:%d\n", i);
	free(*arr);
	(*arr) = NULL;
	//printf("after\n");
	return (ERR_NOMEM);
}

int	is_valid_number(char *str, int sign)
{
	if (sign == 1 && ft_strcmp(str, "9223372036854775807") > 0)
		return (0);
	if (sign == -1 && ft_strcmp(str, "9223372036854775808") > 0)
		return (0);
	return (1);
}