/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_empty_pipe.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:43:37 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/21 22:36:14 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	jump_quote(const char *line, int index)
{
	char	quote;

	quote = line[index];
	index++;
	while (line[index])
	{
		if (line[index] == quote)
			return (index + 1);
		index++;
	}
	return (-1);
}

int	handle_empty_pipe(char *line)
{
	int	i;
	int	last_was_pipe;

	i = 0;
	while (line[i])
	{
		if (line[i] == '\'' || line[i] == '\"')
			i = jump_quote(line, i);
		else if (line[i] == '|')
		{
			if (last_was_pipe)
				return (ERR_SYNTAX);
			last_was_pipe = 1;
		}
		else if (line[i] != ' ' && line[i] != '\t')
			last_was_pipe = 0;
		i++;
	}
	if (last_was_pipe)
		return (ERR_SYNTAX);
	return (0);
}
