/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ubuntu <ubuntu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 18:35:37 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/19 22:04:21 by ubuntu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int		handle_re_output(t_cmd **cmd, char **args, int i, int append);
int		handle_re_input(t_cmd **cmd, char **args, int i);

void	re_add_back(t_red **re, t_red *new)
{
	t_red	*last;

	if (re == NULL || new == NULL)
		return ;
	if (*re == NULL)
		*re = new;
	else
	{
		last = *re;
		while (last->next)
			last = last->next;
		last->next = new;
	}
}

int	process_redirections(t_cmd **cmd, char **args, char *token, int i)
{
	int		error_check;
	t_red	*new;

	error_check = 0;
	new = new_re();
	if (!new)
		return (ERR_NOMEM);
	re_add_back(&(*cmd)->re, new);
	if (!ft_strncmp(">", token, ft_strlen(token)))
		error_check = handle_re_output(cmd, args, i, 0);
	else if (!ft_strncmp("<", token, ft_strlen(token)))
		error_check = handle_re_input(cmd, args, i);
	else if (!ft_strncmp(">>", token, ft_strlen(token)))
		error_check = handle_re_output(cmd, args, i, 1);
	else if (!ft_strncmp("<<", token, ft_strlen(token)))
	{
		error_check = handle_here_doc(cmd, args, i);
		(*cmd)->infile = 1;
	}
	return (error_check);
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
	t_red	*temp;

	if (!args[i + 1] || args[i + 1][0] == '|')
		return (ERR_INVAL);
	temp = (*cmd)->re;
	while (temp)
	{
		if (temp->type == OUT)
			temp->real = 0;
		temp = temp->next;
	}
	temp = (*cmd)->re;
	while (temp->next)
		temp = temp->next;
	temp->file = ft_strdup(args[i + 1]);
	if (!temp->file)
		return (ERR_NOMEM);
	temp->type = OUT;
	temp->real = 1;
	if (append)
		temp->append = 1;
	(*cmd)->outfile = 1;
	(*cmd)->args = remove_n_token((*cmd)->args, i, 2);
	return (0);
}

int	handle_re_input(t_cmd **cmd, char **args, int i)
{
	t_red	*temp;

	if (!args[i + 1] || args[i + 1][0] == '|')
		return (ERR_INVAL);
	temp = (*cmd)->re;
	while (temp->next)
		temp = temp->next;
	temp->file = ft_strdup(args[i + 1]);
	if (!temp->file)
		return (ERR_NOMEM);
	temp->type = IN;
	(*cmd)->infile = 1;
	(*cmd)->args = remove_n_token((*cmd)->args, i, 2);
	return (0);
}
