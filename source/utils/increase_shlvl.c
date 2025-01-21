/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   increase_shlvl.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 18:02:57 by adiler            #+#    #+#             */
/*   Updated: 2025/01/21 18:15:20 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	find_shlvl_index(char **dup_envp)
{
	int	i;

	i = 0;
	while (dup_envp[i])
	{
		if (ft_strncmp(dup_envp[i], "SHLVL=", 6) == 0)
			return (i);
		i++;
	}
	return (-1);
}

int	increase_shlvl(char ***dup_envp)
{
	int		i;
	char	*shlvl;
	int		shlvl_int;

	i = find_shlvl_index(*dup_envp);
	if (i == -1)
		return (0);
	shlvl = ft_strdup((*dup_envp)[i] + 6);
	if (!shlvl)
		return (1);
	shlvl_int = ft_atoi(shlvl);
	free(shlvl);
	shlvl_int++;
	shlvl = ft_itoa(shlvl_int);
	if (!shlvl)
		return (1);
	free((*dup_envp)[i]);
	(*dup_envp)[i] = ft_strjoin("SHLVL=", shlvl);
	if (!(*dup_envp)[i])
		return (1);
	free(shlvl);
	return (0);
}
