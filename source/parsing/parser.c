/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 19:50:24 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/14 15:17:45 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	handle_redirections(t_cmd **cmd)
{
	t_cmd	*temp;
	int		i;
	int		err;

	temp = *cmd;
	err = 0;
	while (*cmd && temp && !err)
	{
		i = 0;
		while (temp->args[i] && !err)
		{
			if (check_redirections(temp->args[i]))
			{
				err = process_redirections(&temp, temp->args, temp->args[i], i);
			}
			else
				i++;
		}
		if (!*cmd || !temp->next)
			break ;
		temp = temp->next;
	}
	return (err);
}

int	fill_everything(char *line, t_cmd **cmd)
{
	int		i;
	t_cmd	*temp;

	i = 0;
	*cmd = new_pipe(tokenizer(line));
	if (!(*cmd))
		return (ERR_NOMEM);
	if (*cmd)
		temp = *cmd;
	while (*cmd && line[i])
	{
		i = next_pipe(line, i);
		if (!line[i])
			break ;
		i++;
		if (line[i])
		{
			temp->next = new_pipe(tokenizer(&line[i]));
			if (!temp->next)
				return (ERR_NOMEM);
		}
		temp = temp->next;
	}
	return (0);
}

int	parser(char **line, t_cmd **cmd)
{
	int	error_check;

	error_check = handle_env_vars(line);
	if (error_check || !line || !*line)
		return (error_check);
	error_check = fill_everything(*line, cmd);
	if (error_check || !*cmd)
		return (error_check);
	error_check = handle_redirections(cmd);
	if (error_check || !*cmd)
		return (error_check);
	return (error_check);
}
