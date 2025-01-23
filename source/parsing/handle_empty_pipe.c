/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_empty_pipe.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 21:43:37 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/23 14:26:59 by adiler           ###   ########.fr       */
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

int	check_initial_pipe(char *line)
{
	int	i;

	i = 0;
	while (line[i] == ' ' || (line[i] >= 9 && line[i] <= 13))
		i++;
	if (line[i] == '|')
		return (ERR_SYNTAX);
	return (0);
}

static int	handle_pipe_char(char *line, int *i, int *last_was_pipe)
{
	if (line[*i] == '\'' || line[*i] == '\"')
	{
		*last_was_pipe = 0;
		*i = jump_quote(line, *i);
	}
	else if (line[*i] == '|')
	{
		if (*last_was_pipe)
			return (ERR_SYNTAX);
		*last_was_pipe = 1;
	}
	else if (line[*i] != ' ' && !(line[*i] >= 9 && line[*i] <= 13))
		*last_was_pipe = 0;
	return (0);
}

int	handle_empty_pipe(char *line)
{
	int i;
	int last_was_pipe;
	int ret;
	int non_space_after_pipe;

	if (check_initial_pipe(line) == ERR_SYNTAX)
		return (ERR_SYNTAX);
	i = 0;
	last_was_pipe = 0;
	non_space_after_pipe = 0;
	while (line[i])
	{
		if (last_was_pipe && line[i] != ' ' && !(line[i] >= 9 && line[i] <= 13))
			non_space_after_pipe = 1;
		ret = handle_pipe_char(line, &i, &last_was_pipe);
		if (ret == ERR_SYNTAX)
			return (ERR_SYNTAX);
		i++;
	}
	if (last_was_pipe && non_space_after_pipe)
		return (ERR_SYNTAX);
	return (0);
}
