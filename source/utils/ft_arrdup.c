/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_2Ddup.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 18:04:37 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/09 19:18:18 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	**ft_arrdup(char **arr)
{
	char	**dup;
	int		i;

	i = 0;
	if (!arr || !*arr)
		return (NULL);
	while (arr[i] && arr[i][0])
		i++;
	dup = malloc(sizeof(char *) * (i + 1));
	if (!dup)
		return (NULL);
	i = 0;
	while (arr[i] && arr[i][0])
	{
		dup[i] = ft_strndup(arr[i], ft_strlen(arr[i]));
		if (!dup[i])
			return (ft_free_arr(&dup), NULL);
		i++;
	}
	dup[i] = NULL;
	return (dup);
}
