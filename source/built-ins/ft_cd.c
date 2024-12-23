/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:14:26 by adiler            #+#    #+#             */
/*   Updated: 2024/12/23 20:21:46 by adiler           ###   ########.fr       */
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

int	builtin_cd(char **args)
{
	char	*path;
	char	*oldpwd;
	char	*pwd;

	oldpwd = getcwd(NULL, 0);
	if (cd_no_arg(&path, args))
		return (1);
	if (cd_old_pwd(&path, args))
		return (1);
	else
		path = args[1];
	if (chdir(path) == -1)
	{
		chdir_error(path);
		return (1);		
	}
	pwd = getcwd(NULL, 0);
	setenv("OLDPWD", oldpwd, 1);
    setenv("PWD", pwd, 1);
	free(oldpwd);
	free(pwd);
	return (0);
}
