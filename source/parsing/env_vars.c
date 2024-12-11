/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 16:44:46 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/11 14:37:38 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	proces_env_var(t_cmd **cmd, int i)
{
	char	*tempura;

	if (!cmd || !(*cmd) || !(*cmd)->args || !(*cmd)->args[i]
		|| !getenv(&((*cmd)->args[i][1])))
		return (ERR_NOMEM);
	tempura = getenv(&((*cmd)->args[i][1]));
	if (!tempura)
		return (ERR_NOMEM);
	free((*cmd)->args[i]);
	(*cmd)->args[i] = ft_strdup(tempura);
	if (!(*cmd)->args[i])
		return(ERR_NOMEM);
	return (0);
}

char	*check_env_vars(char *token)
{
	if (token[0] == '$')
	{
		if (getenv(&token[1]))
			return (getenv(&token[1]));
	}
	return (NULL);
}

int	handle_env_vars(t_cmd **cmd)
{
	t_cmd	*temp;
	int		i;
	int		error_check;

	temp = *cmd;
	error_check = 0;
	while (*cmd && temp && !error_check)
	{
		i = 0;
		while (temp->args[i] && !error_check)
		{
			if (check_env_vars(temp->args[i]))
				error_check = proces_env_var(&temp, i);
			else
				i++;
		}
		if (!*cmd || !temp->next)
			break ;
		temp = temp->next;
	}
	return (error_check);
}
