/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 21:01:16 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/21 21:04:39 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	ft_env(char **envp)
{
	int	i;

	i = 0;
	if (!envp)
		return (1);
	while (envp && envp[i])
	{
		printf("%s\n", envp[i]);
		i++;
	}
	return (0);
}
