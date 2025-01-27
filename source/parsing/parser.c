/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 19:29:24 by adiler            #+#    #+#             */
/*   Updated: 2025/01/23 14:56:04 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	handle_ignore(char *str, const char *chars_to_remove)
{
	char	*read;
	char	*write;
	int		in_dquote;
	int		in_squote;

	in_dquote = -1;
	in_squote = -1;
	read = str;
	write = str;
	while (*read)
	{
		if (*read == '\"' && in_squote == -1)
			in_dquote *= -1;
		if (*read == '\'' && in_dquote == -1)
			in_squote *= -1;
		if (in_squote != -1 || in_dquote != -1 || !ft_strchr(chars_to_remove,
				*read))
		{
			*write = *read;
			write++;
		}
		read++;
	}
	*write = '\0';
}

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

int	parser(char **line, t_cmd **cmd, char **envp, int exit_status)
{
	int	error_check;

	error_check = handle_unclosed_quotes(line);
	if (error_check)
		return (error_check);
	handle_ignore(*line, ";\\");
	error_check = handle_empty_pipe(*line);
	if (error_check)
		return (error_check);
	error_check = handle_trailing_pipe(line);
	if (error_check)
		return (error_check);
	error_check = prep_redirections(line);
	if (error_check)
		return (error_check);
	error_check = handle_env_vars(line, envp, exit_status);
	if (error_check || !line || !*line)
		return (error_check);
	error_check = fill_everything(*line, cmd);
	if (error_check || !*cmd)
		return (error_check);
	return (handle_redirections(cmd));
}
