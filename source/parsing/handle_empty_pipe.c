/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_empty_pipe.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:43:37 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/22 19:48:33 by adiler           ###   ########.fr       */
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
			return (index);
		index++;
	}
	return (-1);
}

int	handle_empty_pipe(char *line)
{
	int	i;
	int	last_was_pipe;

	i = 0;
	last_was_pipe = 0;
	while (line[i])
	{
		if (line[i] == '\'' || line[i] == '\"')
		{
			last_was_pipe = 0;
			i = jump_quote(line, i);
		}
		else if (line[i] == '|')
		{
			if (last_was_pipe)
				return (ERR_SYNTAX);
			last_was_pipe = 1;
		}
		else if (line[i] != ' ' && !(line[i] >= 9 && line[i] <= 13))
			last_was_pipe = 0;
		i++;
	}
	if (last_was_pipe)
		return (ERR_SYNTAX);
	return (0);
}
