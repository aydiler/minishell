/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unclosed_quotes.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/20 14:56:08 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/20 16:20:43 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	check_quotes(char *line, int )
{
	int		in_quote;
	int		i;
	char	id;

	in_quote = -1;
	i = 0;
	while (line[i])
	{
		if (line[i] == '\"' || line[i] == '\'')
		{
			id = line[i];
			in_quote *= -1;
			i++;
			while (line[i] && line[i] != id)
				i++;
			if (line[i] != id)
				break ;
			in_quote *= -1;
		}
		i++;
	}
	if (in_quote == 1)
		return (id);
	return (0);
}

int	handle_unclosed_quotes(char **line)
{
	char	*extra_input;
	char	id;
	char	*tmp;

	id = check_quotes(*line, 0);
	if (!id)
		return (0);
	while (1)
	{
		tmp = *line;
		extra_input = readline("> ");
		*line = ft_strjoin(tmp, "\n");
		free(tmp);
		tmp = *line;
		*line = ft_strjoin(tmp, extra_input);
		free(tmp);
		printf("line: %s\n", *line);
		if (ft_strchr(extra_input, id))
			break ;
		free(extra_input);
	}
	free(extra_input);
	return (0);
}
