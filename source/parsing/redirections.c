/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 18:35:37 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/10 14:56:57 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	handle_re_output(t_cmd **cmd, char **args, int i);
int	handle_re_input(t_cmd **cmd, char **args, int i);
int	handle_ap_output(t_cmd **cmd, char **args, int i);

int	process_redirections(t_cmd **cmd, char **args, char *token, int i)
{
	int	error_check;

	error_check = 0;
	if (!ft_strncmp(">", token, ft_strlen(token)))
		error_check = handle_re_output(cmd, args, i);
	else if (!ft_strncmp("<", token, ft_strlen(token)))
		error_check = handle_re_input(cmd, args, i);
	else if (!ft_strncmp(">>", token, ft_strlen(token)))
		error_check = handle_ap_output(cmd, args, i);
	if (error_check)
		return (error_check);
	return (0);
}

int	check_redirections(char *token)
{
	if (!token[0])
		return (0);
	if (!ft_strncmp(">", token, ft_strlen(token)))
		return (1);
	else if (!ft_strncmp("<", token, ft_strlen(token)))
		return (1);
	else if (!ft_strncmp(">>", token, ft_strlen(token)))
		return (1);
	return (0);
}

int	handle_re_output(t_cmd **cmd, char **args, int i)
{
	int	j;

	j = 0;
	if (!args[i + 1] || args[i + 1][0] == '|')
		return (1);
	if ((*cmd)->output_file)
	{
		while ((*cmd)->files_to_create && (*cmd)->files_to_create[j])
			j++;
		(*cmd)->files_to_create = ft_realloc((*cmd)->files_to_create, (j + 2));
		if (!(*cmd)->files_to_create)
			return (1);
		(*cmd)->files_to_create[j] = ft_strdup((*cmd)->output_file);
		if (!(*cmd)->files_to_create[j])
			return (1);
		(*cmd)->files_to_create[j + 1] = NULL;
		free((*cmd)->output_file);
	}
	(*cmd)->output_file = ft_strdup(args[i + 1]);
	if (!(*cmd)->output_file)
		return (1);
	(*cmd)->args = remove_n_token((*cmd)->args, i, 2);
	return (0);
}

int	handle_re_input(t_cmd **cmd, char **args, int i)
{
	if (!args[i + 1])
		return (1);
	(*cmd)->input_file = ft_strdup(args[i + 1]);
	if (!(*cmd)->input_file)
		return (1);
	(*cmd)->args = remove_n_token((*cmd)->args, i, 2);
	return (0);
}

int	handle_ap_output(t_cmd **cmd, char **args, int i)
{
	int	j;

	j = 0;
	if (!args[i + 1])
		return (1);
	if ((*cmd)->output_file)
	{
		while ((*cmd)->files_to_create && (*cmd)->files_to_create[j])
			j++;
		(*cmd)->files_to_create = ft_realloc((*cmd)->files_to_create, (j + 2));
		if (!(*cmd)->files_to_create)
			return (1);
		(*cmd)->files_to_create[j] = ft_strdup((*cmd)->output_file);
		if (!(*cmd)->files_to_create[j])
			return (1);
		(*cmd)->files_to_create[j + 1] = NULL;
		free((*cmd)->output_file);
	}
	(*cmd)->output_file = ft_strdup(args[i + 1]);
	if (!(*cmd)->output_file)
		return (1);
	(*cmd)->append_outfile = 1;
	(*cmd)->args = remove_n_token((*cmd)->args, i, 2);
	return (0);
}
