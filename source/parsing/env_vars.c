/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 16:44:46 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/22 20:03:34 by adiler           ###   ########.fr       */
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

	if (!ft_strchr(*line, '$') || is_exit_status_var(*line))
		return (0);
	error_check = 0;
	i = 0;
	len_var = 0;
	while ((*line)[i] && (*line)[i] != '$')
		i++;
	env_var = getenv_range(&((*line)[i + 1]), &len_var, envp);
	if (!env_var)
		return (ERR_NOMEM);
	error_check = exchange_var(line, env_var, i, len_var);
	free(env_var);
	return (error_check);
}

int	check_env_vars(char **line, char **envp, int exit_status)
{
	int		start;
	int		end;
	char	*tmp;
	char	*tmp2;

	if (has_env_var(*line))
		return (has_env_var(*line));
	if (is_exit_status_var(*line))
		return (fill_in_exit_status(line, exit_status));
	start = find_var_start(*line);
	if (!start)
		return (1);
	end = start;
	while (is_env_var((*line)[end]))
		end++;
	tmp = ft_strndup(&((*line)[start]), end - start);
	tmp2 = ft_getenv(tmp, envp);
	if (tmp2)
		return (ft_memdel((void **)&(tmp)), ft_memdel((void **)&(tmp2)), 0);
	return (ft_memdel((void **)&(tmp)), ft_memdel((void **)&(tmp2)), NOVAR);
}

int	handle_env_vars(char **line, char **envp, int exit_status)
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
	var_check = 0;
	while ((!var_check || var_check == -1 || var_check == -3) && !error_check 
		&& line && *line)
	{
		var_check = check_env_vars(line, envp, exit_status);
		if (var_check > 0)
			break ;
		if (!var_check)
			error_check = proces_env_var(line, envp);
		else
			error_check = remove_false_var(line, var_check);
		if (error_check)
			return (error_check);
	}
	return (error_check);
}
