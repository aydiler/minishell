/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 16:44:46 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/23 21:13:17 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	exchange_var(char **line, char *env_var, int i, size_t len_var)
{
	size_t	new_len;
	char	*new_str;

	new_len = (ft_strlen(*line) - (len_var + 1)) + ft_strlen(env_var);
	new_str = malloc(sizeof(char) * new_len + 1);
	if (!new_str)
		return (ERR_NOMEM);
	ft_strncpy(new_str, *line, i);
	ft_strncpy(new_str + i, env_var, ft_strlen(env_var));
	ft_strncpy(new_str + i + ft_strlen(env_var), (*line) + i + len_var + 1, 
		ft_strlen((*line) + i + len_var + 1));
	new_str[new_len] = '\0';
	ft_memdel((void **)&(*line));
	*line = ft_strdup(new_str);
	ft_memdel((void **)&(new_str));
	return (0);
}

int	handle_tilde(char **line)
{
	int	i;
	int	error_check;

	i = 0;
	error_check = 0;
	while ((*line)[i])
	{
		if (is_tilde(*line, i))
		{
			error_check = exchange_tilde(line, i);
			if (error_check)
				return (error_check);
		}
		if ((*line)[i] == '\'' || (*line)[i] == '\"')
		{
			error_check = skip_quote(*line, &i);
			if (error_check)
				return (error_check);
		}
		i++;
	}
	return (0);
}

int	proces_env_var(char **line, char **envp)
{
	char	*env_var;
	int		i;
	size_t	len_var;
	int		error_check;

	error_check = 0;
	i = 0;
	len_var = 0;
	while ((*line)[i] && (*line)[i] != '$')
		i++;
	if (!getenv_range(&((*line)[i + 1]), &len_var, envp))
		return (ERR_ENV_VAR);
	env_var = getenv_range(&((*line)[i + 1]), &len_var, envp);
	if (!env_var)
		return (ERR_NOMEM);
	error_check = exchange_var(line, env_var, i, len_var);
	return (error_check);
}

int	check_env_vars(char *line, char **envp)
{
	int		start;
	int		end;
	char	*temp;

	if (ft_strchr(line, '$'))
	{
		start = 0;
		while (line[start] != '$')
		{
		// 	if (line[start] == '\'')
		// 		start == jump_s_quote(line, start);
			start++;
		}
		start++;
		end = start;
		while (is_env_var(line[end]))
			end++;
		temp = ft_strn(&(line[start]), end - start);
		if (!temp)
			return (ERR_NOMEM);
		if (ft_getenv(temp, envp))
			return (ft_memdel((void **)&(temp)), 0);
		else
			return (ft_memdel((void **)&(temp)), ERR_ENV_VAR);
	}
	return (1);
}

int	handle_env_vars(char **line, char **envp)
{
	int		error_check;
	int		var_check;

	error_check = 0;
	if (!line || !*line)
		return (error_check);
	if (ft_strchr(*line, '~'))
		error_check = handle_tilde(line);
	if (error_check)
		return (error_check);
	var_check = check_env_vars(*line, envp);
	if (var_check && var_check != 1)
		return (var_check);
	while (!var_check && !error_check && line && *line)
	{
		error_check = proces_env_var(line, envp);
		if (error_check)
			return (error_check);
		var_check = check_env_vars(*line, envp);
		if (var_check && var_check != 1)
			return (var_check);
	}
	return (error_check);
}
