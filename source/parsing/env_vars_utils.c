/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 17:37:49 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/11 17:56:05 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	is_env_var(char c)
{
	if (c == ' ' || (c >= '9' && c <= '13') || c == '|' || c == '\0' || 
		c == '$' || c == ';' || c == '\\')
		return (0);
	return (1);
}

char	*get_var_env(char *str)
{
	char	*temp;
	int		i;
	int		start;
	int		end;

	start = 0;
	while (line[start] != '$')
		start++;
	end = start;
	while (is_env_var(line[end]))
		end++;
	if (!str || start < 0 || end < start)
		return (NULL);
	temp = malloc (sizeof(char) * (end - start + 1));
	if (!temp)
		return (NULL);
	ft_strncpy(temp, &str[start], end - start + 1);
	temp[end - start + 1] = '\0';
	getenv(temp)
}
