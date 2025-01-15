/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 16:20:36 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/10 20:47:48 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	find_var_start(char *line)
{
	int	start;

	start = 0;
	while (line[start] && line[start] != '$')
	{
		if (line[start] == '\'')
			start = jump_s_quote(line, start);
		else
			start++;
	}
	if (!(line)[start])
		return (0);
	start++;
	return (start);
}

int	is_exit_status_var(char *line)
{
	char	*dollar;

	dollar = ft_strchr(line, '$');
	return (dollar && *(dollar + 1) == '?');
}

int	has_env_var(char *line)
{
	char	*dollar;

	dollar = ft_strchr(line, '$');
	return (dollar && *(dollar + 1) && *(dollar + 1) != ' ' 
		&& !(*(dollar + 1) >= 9 && *(dollar + 1) <= 13));
}

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

int	fill_in_exit_status(char **line, int exit_status)
{
	char	*exit;
	char	*new_str;
	int		i;

	i = 0;
	while ((*line)[i] && (*line)[i] != '$')
		i++;
	exit = ft_itoa(exit_status);
	new_str = malloc(sizeof(char) * (ft_strlen(*line) + ft_strlen(exit)));
	if (!new_str)
		return (ERR_NOMEM);
	ft_strncpy(new_str, *line, i);
	ft_strncpy(new_str + i, exit, ft_strlen(exit));
	ft_strncpy(new_str + i + ft_strlen(exit), (*line) + i + 2, 
		ft_strlen((*line) + i + 2));
	new_str[ft_strlen(*line) + ft_strlen(exit) - 2] = '\0';
	ft_memdel((void **)&(*line));
	*line = ft_strdup(new_str);
	ft_memdel((void **)&(new_str));
	ft_memdel((void **)&(exit));
	return (0);
}
