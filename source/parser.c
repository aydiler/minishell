/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 19:50:24 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/27 20:56:48 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	next_pipe(char *line, int i)
{
	while (line[i] && line[i] != '|')
		i++;
	return (i);
}

t_cmd	*new_pipe(char **args)
{
	t_cmd	*pipe;

	pipe = malloc (sizeof(t_cmd));
	if (!pipe)
		return (NULL);
	pipe->args = args;
	pipe->next = NULL;
	return (pipe);
}

void	parser(char *line, t_cmd **cmd)
{
	int		i;
	t_cmd	*temp;

	i = 0;
	*cmd = new_pipe(tokenizer(line));
	if (!(*cmd))
		return ;
	temp = *cmd;
	while (line[i])
	{
		i = next_pipe(line, i);
		if (!line[i])
			break ;
		if (line[i + 1])
			temp->next = new_pipe(tokenizer(&line[i + 1]));
		if (!temp->next)
			break ;
		temp = temp->next;
	}
}
