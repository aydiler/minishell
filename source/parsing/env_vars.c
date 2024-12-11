/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 16:44:46 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/11 17:37:10 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	is_env_var(char c)
{
	if (c == ' ' || (c >= '9' && c <= '13') || c == '|' || c == '\0' || 
		c == '$')
		return (0);
	return (1);
}


char	*ft_replace(char *str, char c, char *re)
{
	char	*pos;
	char	*new_str;
	size_t	len_pre;
	size_t	len_re;
	size_t	new_len;

	pos = strchr(str, c);
	len_pre = str - pos;
	len_re = ft_strlen(re);
	new_len = ft_strlen(str) + len_re - 1;

	new_str = malloc(new_len * sizeof(char));
	if (!new_str)
		return (NULL);
	return (new_str);
}

int	proces_env_var(char **line)
{
	char	*tempura;
	int		i;

	i = 0;
	while (*line[i] && *line[i] != '$')
		j++;
	if (!getenv(&(*line[i + 1])))
		return (ERR_ENV_VAR);
	tempura = getenv(&((*cmd)->args[i][j + 1]));
	if (!tempura)
		return (ERR_NOMEM);
	free((*cmd)->args[i]);
	(*cmd)->args[i] = ft_strdup(tempura);
	if (!(*cmd)->args[i])
		return (ERR_NOMEM);
	return (0);
}

int	check_env_vars(char *line)
{
	int	start;
	int	end;

	if (ft_strchr(line, '$'))
	{
		start = 0;
		while (line[start] != '$')
			start++;
		start++;
		end = start;
		while (is_env_var(line[end]))
			end++;
		if (getenv(line))
			return (0);
		else
			return (ERR_ENV_VAR);
	}
	return (1);
}

int	handle_env_vars(char **line)
{
	int		error_check;

	error_check = 0;
	if (!line || !*line)
		return (error_check);
	if (ft_strchr(*line, '~'))
		error_check = handle_tilde(line);
	if (error_check)
		return (error_check);
	error_check = check_env_vars(*line);
	if (!error_check && line && *line)
		error_check = proces_env_var(line);
	return (error_check);
}
