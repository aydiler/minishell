/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 18:35:37 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/18 21:10:46 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	handle_re_output(t_cmd **cmd, char **args, int i, int append);
int	handle_re_input(t_cmd **cmd, char **args, int i);

int	process_redirections(t_cmd **cmd, char **args, char *token, int i)
{
	int	error_check;

	error_check = 0;
	while ((*cmd)->re)
		(*cmd)->re = (*cmd)->re->next;
	(*cmd)->re = new_re();
	if (!(*cmd)->re)
		return (ERR_NOMEM);
	if (!ft_strncmp(">", token, ft_strlen(token)))
		error_check = handle_re_output(cmd, args, i, 0);
	else if (!ft_strncmp("<", token, ft_strlen(token)))
		error_check = handle_re_input(cmd, args, i);
	else if (!ft_strncmp(">>", token, ft_strlen(token)))
		error_check = handle_re_output(cmd, args, i, 1);
	else if (!ft_strncmp("<<", token, ft_strlen(token)))
		error_check = handle_here_doc(cmd, args, i);
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
	else if (!ft_strncmp("<<", token, ft_strlen(token)))
		return (1);
	return (0);
}

int	handle_re_output(t_cmd **cmd, char **args, int i, int append)
{
	if (!args[i + 1] || args[i + 1][0] == '|')
		return (ERR_INVAL);
	while ((*cmd)->re->next)
		(*cmd)->re = (*cmd)->re->next;
	(*cmd)->re->file = ft_strdup(args[i + 1]);
	if (!(*cmd)->re->file)
		return (ERR_NOMEM);
	(*cmd)->re->type = OUT;
	if (append)
		(*cmd)->re->append = 1;
	(*cmd)->args = remove_n_token((*cmd)->args, i, 2);
	return (0);
}

int	handle_re_input(t_cmd **cmd, char **args, int i)
{
	if (!args[i + 1] || args[i + 1][0] == '|')
		return (ERR_INVAL);
	while ((*cmd)->re->next)
		(*cmd)->re = (*cmd)->re->next;
	(*cmd)->re->file = ft_strdup(args[i + 1]);
	if (!(*cmd)->re->file)
		return (ERR_NOMEM);
	(*cmd)->re->type = IN;
	(*cmd)->args = remove_n_token((*cmd)->args, i, 2);
	return (0);
}
