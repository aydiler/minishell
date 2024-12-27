/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:15:24 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/27 18:17:21 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	print_env(char **envp)
{
	char	**start;
	int		i;
	char	*equal;
	char	*name;

	start = envp;
	envp = ft_sort_envp(&envp);
	i = -1;
	while (envp[++i])
	{
		equal = ft_strdup(ft_strchr(envp[i], '='));
		if (!equal)
			return (0);
		name = ft_strn(envp[i], (int)(ft_strlen(envp[i]) - ft_strlen(equal)));
		printf("declare -x %s=\"%s\"\n", name, equal + 1);
		free(equal);
		free(name);
	}
	envp = start;
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
			(*envp)[i] = new_env_var;
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
			return (ft_free_arr(new_envp, j));
	}
	new_envp[i] = ft_strdup(new_env_var);
	if (!new_envp[i])
		return (ft_free_arr(new_envp, i));
	new_envp[i + 1] = NULL;
	*envp = new_envp;
	ft_free_arr(new_envp, (i + 1));
	return (0);
}

int	add_replace_env(char ***envp, char **args)
{
	int		error_check;
	char	*equal;
	char	*name;

	equal = ft_strdup(ft_strchr(args[1], '='));
	if (!equal)
		return (0);
	name = malloc(sizeof(char) * (ft_strlen(equal) - 1));
	if (!name)
		return (ERR_NOMEM);
	name = ft_strncpy(name, args[1], ft_strlen(equal) - 1);
	if (ft_getenv(name, *envp))
		replace_env(envp, args[1], name);
	else
		error_check = add_env(envp, args[1]);
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
