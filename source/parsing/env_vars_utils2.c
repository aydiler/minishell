/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 16:20:36 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/09 17:19:49 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	remove_false_var(char **line)
{
	int		i;
	int		j;
	char	*line_before;
	char	*line_after;

	i = 0;
	while ((*line)[i] && (*line)[i] != '$')
		i++;
	j = i + 1;
	if ((*line)[i])
	{
		while (is_env_var((*line)[j]))
			j++;
	}
	line_before = ft_strndup(*line, i);
	line_after = ft_strdup(&(*line)[j]);
	free(*line);
	*line = ft_strjoin(line_before, line_after);
	if (!(line))
		return (ERR_NOMEM);
	free(line_before);
	free(line_after);
	return (0);
}
