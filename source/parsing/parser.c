/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 19:50:24 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/05 19:52:37 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	process_non_args(t_cmd **cmd, char **args, char *token, int i)
{
	int	error_check;

	error_check = 0;
	if (!ft_strncmp(">", token, ft_strlen(token)))
	{
		error_check = handle_re_output(cmd, args, i);
	}
	else if (!ft_strncmp("<", token, ft_strlen(token)))
	{
		error_check = handle_re_input(cmd, args, i);
	}
	else if (!ft_strncmp(">>", token, ft_strlen(token)))
	{
		error_check = handle_ap_output(cmd, args, i);
	}
	else if (!ft_strncmp(";", token, ft_strlen(token) || 
			!ft_strncmp("\\", token, ft_strlen(token))))
		(*cmd)->args = remove_token((*cmd)->args, i);
	if (error_check)
		return (error_check);
	return (0);
}

int	handle_non_args(t_cmd **cmd)
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
			if (check_non_args(temp->args[i]))
				err = process_non_args(&temp, temp->args, temp->args[i], i);
			else
				i++;
		}
		if (!*cmd || !temp->next)
			break ;
		temp = temp->next;
	}
	return (err);
}

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
	pipe->input_file = NULL;
	pipe->output_file = NULL;
	pipe->append_outfile = 0;
	pipe->files_to_create = NULL;
	pipe->next = NULL;
	return (pipe);
}

int	parser(char *line, t_cmd **cmd)
{
	int		i;
	t_cmd	*temp;

	i = 0;
	*cmd = new_pipe(tokenizer(line));
	if (!(*cmd))
		return (1);
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
				return (1);
		}
		temp = temp->next;
	}
	return (handle_non_args(cmd));
}
