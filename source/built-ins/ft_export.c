/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:15:24 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/23 14:53:21 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	print_env(char **envp)
{
	char	**tmp;
	int		i;
	char	*equal;
	char	*name;

	tmp = ft_sort_envp(envp);
	i = -1;
	while (tmp[++i])
	{
		if (!ft_strchr(tmp[i], '='))
		{
			printf("declare -x %s\n", tmp[i]);
			continue ;
		}
		equal = ft_strdup(ft_strchr(tmp[i], '='));
		if (!equal)
			return (ft_free_arr(&tmp), ERR_NOMEM);
		name = ft_strndup(tmp[i], ft_strlen(tmp[i]) - ft_strlen(equal));
		printf("declare -x %s=\"%s\"\n", name, equal + 1);
		free(equal);
		free(name);
	}
	ft_free_arr(&tmp);
	return (0);
}

void	replace_env(char ***envp, char *new_env_var, char *name)
{
	int	i;

	i = 0;
	while ((*envp)[i])
	{
		if (!strncmp((*envp)[i], name, ft_strlen(name)) && 
			(*envp)[i][ft_strlen(name)] == '=')
		{
			free((*envp)[i]);
			(*envp)[i] = ft_strdup(new_env_var);
			return ;
		}
		i++;
	}
}

int	add_env(char ***envp, char *new_env_var)
{
	int		i;
	int		j;
	char	**new_envp;

	i = 0;
	j = -1;
	while ((*envp)[i])
		i++;
	new_envp = malloc(sizeof(char *) * (i + 2));
	if (!new_envp)
		return (ERR_NOMEM);
	while (++j < i)
	{
		new_envp[j] = ft_strdup((*envp)[j]);
		if (!new_envp[j])
			return (ft_free_arr(&new_envp));
	}
	new_envp[i] = ft_strdup(new_env_var);
	if (!new_envp[i])
		return (ft_free_arr(&new_envp));
	new_envp[i + 1] = NULL;
	ft_free_arr(envp);
	*envp = new_envp;
	return (0);
}

int	add_replace_env(char ***envp, char **args)
{
	int		error_check;
	char	*equal;
	char	*name;
	char	*tmp;

	error_check = 0;
	if (!ft_strchr(args[1], '='))
		return (add_env(envp, args[1]), 0);
	equal = ft_strdup(ft_strchr(args[1], '='));
	name = ft_strndup(args[1], (ft_strlen(args[1]) - ft_strlen(equal)));
	tmp = ft_getenv(name, *envp);
	if (tmp)
	{
		replace_env(envp, args[1], name);
		free(tmp);
	}
	else
		error_check = add_env(envp, args[1]);
	free(equal);
	free(name);
	return (error_check);
}

int	ft_export(char ***envp, char **args)
{
	int		error_check;
	char	**temp;

	error_check = 0;
	if (!envp || !*envp || !**envp)
		return (1);
	temp = args;
	if (!args[1])
		error_check = print_env(*envp);
	else
	{
		while (*args && *(args + 1))
		{
			if (!is_valid_env_name(*(args + 1)))
				return (print_export_error(*(args + 1)));
			error_check = add_replace_env(envp, args);
			args++;
		}
	}
	args = temp;
	return (error_check);
}
