/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:15:24 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/29 11:05:31 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	print_env(char **envp)
{
	char	**temp;
	int		i;
	char	*equal;
	char	*name;

	temp = ft_2Ddup(envp);
	if (!temp)
		return (ERR_NOMEM);
	temp = ft_sort_envp(envp);
	i = -1;
	while (temp[++i])
	{
		equal = ft_strdup(ft_strchr(envp[i], '='));
		if (!equal)
			return (ERR_NOMEM);
		name = ft_strndup(temp[i], (int)(ft_strlen(temp[i]) - ft_strlen(equal)));
		if (!name)
			return (ERR_NOMEM);
		printf("%ddeclare -x %s=\"%s\"\n", i, name, equal + 1);
		free(equal);
		free(name);
	}
	ft_free_arr(&temp);
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
	*envp = ft_2Ddup(new_envp);
	ft_free_arr(&new_envp);
	return (0);
}

int	add_replace_env(char ***envp, char **args)
{
	int		error_check;
	char	*equal;
	char	*name;
	char	*temp;

	equal = ft_strdup(ft_strchr(args[1], '='));
	if (!equal)
		return (0);
	name = ft_strndup(args[1], (ft_strlen(args[1]) - ft_strlen(equal)));
	if (!name)
		return (ERR_NOMEM);
	temp = ft_getenv(name, *envp);
	if (temp)
	{
		replace_env(envp, args[1], name);
		free(temp);
	}
	else
		error_check = add_env(envp, args[1]);
	free(equal);
	free(name);
	return (error_check);
}

int	ft_export(char ***envp, char **args)
{
	int	error_check;

	error_check = 0;
	if (!envp || !*envp || !**envp)
		return (1);
	if (!args[1])
		error_check = print_env(*envp);
	else
		error_check = add_replace_env(envp, args);
	return (error_check);
}
