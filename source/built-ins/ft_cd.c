/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:14:26 by adiler            #+#    #+#             */
/*   Updated: 2024/12/24 23:18:15 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	cd_no_arg(char **path, char **args)
{
	if(!args[1])
	{
		*path = getenv("HOME");
		if (!(*path))
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", 2);
			return (1);
		}
	}
	return (0);
}

int cd_old_pwd(char **path, char **args)
{
	if(args[1][0] == '-' && !args[1][1])
	{
		*path = getenv("OLDPWD");
		if (!(*path))
		{
			ft_putstr_fd("minishell: cd: OLDPWD not set\n", 2);
			return (1);
		}
		ft_putstr_fd(*path, 1);
		ft_putstr_fd("\n", 1);
	}
	return (0);
}

void chdir_error(char *path)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd("cd: ", 2);
	ft_putstr_fd(path, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd(strerror(errno), 2);
	ft_putstr_fd("\n", 2);
}

int set_path(char **path, char **args)
{
	if (!args[1])
		return cd_no_arg(path, args);
	else if (args[1][0] == '-' && !args[1][1])
		return cd_old_pwd(path, args);
	else if (args[1])
	{
		*path = args[1];
		return 0;
	}
	return 1;
}

int ft_cd(char **args)
{
	char *path;
	char *oldpwd;
	char *pwd;

	if(set_path(&path, args))
		return (1);
	oldpwd = getcwd(NULL, 0);
	if (chdir(path) == -1)
	{
		chdir_error(path);
		free(oldpwd);
		return (1);
	}
	pwd = getcwd(NULL, 0);
	setenv("OLDPWD", oldpwd, 1);
	setenv("PWD", pwd, 1);
	free(oldpwd);
	free(pwd);
	return (0);
}
