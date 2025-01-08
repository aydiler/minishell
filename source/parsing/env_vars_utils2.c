/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 16:20:36 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/08 17:09:04 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	remove_false_var(char **line)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	while ((*line)[i] && (*line)[i] != '$')
		i++;
	if ((*line)[i])
	{
		j = i + 1;
		while (is_env_var((*line[j])))
			j++;
	}
	else
		return (8723);
	temp = *line;
	ft_memdel((void **)line);
	*line = malloc(sizeof(char *) * ft_strlen(temp) - (j - i));
	if (!(*line))
		return (ERR_NOMEM);
	ft_strncpy(*line, temp, i + 1);
	if (!(*line))
		return (ERR_NOMEM);
	// rest hinzufuegen
	free(temp);
	return (0);
}
