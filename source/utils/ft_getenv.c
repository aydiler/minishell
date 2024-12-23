/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_getenv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 20:09:54 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/23 20:27:34 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	*ft_getenv(char *name, char **envp)
{
	int		i;
	char	*value;

	i = 0;
	if (!name || !*name || !envp)
		return (NULL);
	while (envp[i])
	{
		if (!ft_strncmp(name, envp[i], ft_strlen(name)) && 
			envp[i][ft_strlen(name)] == '=')
			return (ft_strdup(ft_strchr(envp[i], '=') + 1));
		i++;
	}
	return (NULL);
}
