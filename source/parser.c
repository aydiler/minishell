/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 19:50:24 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/30 20:10:14 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	process_non_args(t_cmd **cmd, char **args, char *token, int i)
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
		ft_error(*cmd, NULL);
		// error functions!!!
}

void	handle_non_args(t_cmd **cmd)
{
	t_cmd	*temp;
	int		i;

	temp = *cmd;
	while (temp)
	{
		i = 0;
		while (temp->args[i])
		{
			if (check_non_args(temp->args[i]))
				process_non_args(&temp, temp->args, temp->args[i], i);
			else
				i++;
		}
		if (!temp->next)
			break ;
		temp = temp->next;
	}
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

void	parser(char *line, t_cmd **cmd)
{
	int		i;
	t_cmd	*temp;

	i = 0;
	*cmd = new_pipe(tokenizer(line));
	if (!(*cmd))
		ft_error(*cmd, NULL);
	temp = *cmd;
	while (line[i])
	{
		i = next_pipe(line, i);
		if (!line[i])
			break ;
		i++;
		if (line[i])
		{
			temp->next = new_pipe(tokenizer(&line[i]));
			if (!temp->next)
				ft_error(*cmd, NULL);
		}
		temp = temp->next;
	}
	handle_non_args(cmd);
}
