/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 13:53:50 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/18 21:59:56 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	add_spaces(char **line, int *i, int end)
{
	int		error_check;
	char	*add;
	char	*temp;

	error_check = 0;
	add = ft_strndup(&(*line)[*i], end - *i);
	if (!add)
		return (ERR_NOMEM);
	temp = ft_strdup(*line);
	if (!temp)
		return (free(add), ERR_NOMEM);
	free(*line);
	*line = malloc(sizeof(char) * (ft_strlen(temp) + ft_strlen(add) + 2));
	if (!*line)
		return (ERR_NOMEM);
	ft_strncpy(*line, temp, *i);
	(*line)[*i] = '\0';
	ft_strcat(*line, " ");
	ft_strcat(*line, add);
	ft_strcat(*line, " ");
	ft_strcat(*line, &temp[end]); 
	free(temp);
	*i += ft_strlen(add) + 1;
	free(add);
	return (error_check);
}

int	prep_redirections(char **line)
{
	int	error_check;
	int	i;

	i = 0;
	error_check = 0;
	while ((*line)[i])
	{
		if ((*line)[i] == '\"' || (*line)[i] == '\'')
			skip_quote(*line, &i);
		if ((*line)[i] == '<' && (*line)[i + 1] && (*line)[i + 1] == '<')
			error_check = add_spaces(line, &i, i + 2);
		else if ((*line)[i] == '<')
			error_check = add_spaces(line, &i, i + 1);
		else if ((*line)[i] == '>' && (*line)[i + 1] && (*line)[i + 1] == '>')
			error_check = add_spaces(line, &i, i + 2);
		else if ((*line)[i] == '>')
			error_check = add_spaces(line, &i, i + 1);
		if (error_check)
			return (error_check);
		i++;
	}
	return (error_check);
}

t_red	*new_re(void)
{
	t_red	*re;

	re = malloc(sizeof(t_red));
	if (!re)
		return (NULL);
	re->file = NULL;
	re->type = 0;
	re->append = 0;
	re->real = 0;
	re->next = NULL;
	return (re);
}
