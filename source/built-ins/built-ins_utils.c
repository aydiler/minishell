/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built-ins_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 16:18:06 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/15 22:04:33 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	print_export_error(char *name)
{
	ft_putstr_fd("bash: export: `", 2);
	write(2, name, ft_strlen(name));
	ft_putstr_fd("': not a valid identifier\n", 2);
	return (1);
}

int	is_valid_env_name(char *name)
{
	int	i;

	i = 0;
	if (!name[i] || !(name[i] == '_' || ft_isalpha(name[i])))
		return (0);
	i++;
	while (name[i] && name[i] != '=')
	{
		if (!(ft_isdigit(name[i]) || ft_isalpha(name[i]) || name[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

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

int	is_valid_number(char *str, int sign)
{
	if (sign == 1 && ft_strcmp(str, "9223372036854775807") > 0)
		return (0);
	if (sign == -1 && ft_strcmp(str, "9223372036854775808") > 0)
		return (0);
	return (1);
}
