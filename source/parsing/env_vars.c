/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 16:44:46 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/12 17:25:47 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	exchange_var(char **line, char *env_var, int i, size_t len_var)
{
	size_t	new_len;
	char	*new_str;

	new_len = (ft_strlen(*line) - (len_var + 1)) + ft_strlen(env_var);
	new_str = malloc(sizeof(char) * new_len);
	if (!new_str)
		return (ERR_NOMEM);
	ft_strncpy(new_str, *line, i);
	ft_strncpy(new_str + i, env_var, ft_strlen(env_var));
	ft_strncpy(new_str + i + ft_strlen(env_var), *line + i + len_var + 1, 
		ft_strlen(*line + i + len_var + 1));
	new_str[new_len] = '\0';
	free(*line);
	*line = new_str;
	free(new_str);
	return (0);
}

int	handle_tilde(char **line)
{
	int	i;
	int	error_check;

	i = 0;
	error_check = 0;
	while (*line[i])
	{
		if (is_tilde(*line, i))
		{
			error_check = exchange_tilde(line, i);
			if (error_check)
				return (error_check);
		}
		if ((*line)[i] == '\"' || *line[i] == '\'')
		{
			error_check = skip_quote(*line, &i);
			if (error_check)
				return (error_check);
		}
		i++;
	}
	return (0);
}


int	proces_env_var(char **line)
{
	char	*env_var;
	int		i;
	size_t	len_var;
	int		error_check;

	error_check = 0;
	i = 0;
	len_var = 0;
	while (*line[i] && *line[i] != '$')
		i++;
	if (!getenv_range(&(*line[i + 1]), &len_var))
		return (ERR_ENV_VAR);
	env_var = getenv_range(&(*line[i + 1]), &len_var);
	if (!env_var)
		return (ERR_NOMEM);
	error_check = exchange_var(line, env_var, i, len_var);
	return (error_check);
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
