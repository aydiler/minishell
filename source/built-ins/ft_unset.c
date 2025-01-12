/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 17:38:47 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/12 18:02:48 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	ft_unsetenv(char ***envp, char *name)
{
	int	i;
	
	i = 0;
	while ((*envp)[i])
	{
		if (!ft_strncmp((*envp)[i], name, ft_strlen(name)) && 
			(*envp)[i][ft_strlen(name) == '='])
		{
			ft_memdel((void **)((*envp)[i]));
			while ((*envp)[i])
			{
				(*envp)[i] = (*envp)[i + 1];
				i++;
			}
			return (0);
		}
		i++;
	}
	return (0);
}

int	ft_unset(char ***envp, char **args)
{
	int	i;
	int	error_check;

	i = 1;
	if (!args || !args[0] || !args[1])
		return (0);
	while (args[i])
	{
		error_check = ft_unsetenv(envp, args[i]);
		if (error_check)
			return (error_check);
		i++;
	}
	return (error_check);
}
