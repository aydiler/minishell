/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 18:14:26 by adiler            #+#    #+#             */
/*   Updated: 2024/12/22 19:39:25 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	builtin_cd(char **args)
{
	char	*path;
	char	*oldpwd;
	char	*pwd;

	printf("cd is called\n");
	oldpwd = getcwd(NULL, 0);
	// If no argument is provided, change to HOME directory
	if (!args[1])
	{
		printf("no args\n");
		path = getenv("HOME");
		if (!path)
		{
			ft_putstr_fd("cd: HOME not set\n", 2);
			return (1);
		}
	}
	// If argument is "-", change to OLDPWD
	else if (args[1][0] == '-' && !args[1][1])
	{
		path = getenv("OLDPWD");
		if (!path)
		{
			ft_putstr_fd("cd: OLDPWD not set\n", 2);
			return (1);
		}
		ft_putstr_fd(path, 1);
		ft_putstr_fd("\n", 1);
	}
	// Otherwise, change to the specified directory
	else
		path = args[1];
	// Change directory
	printf("path: %s\n", path);
	if (chdir(path) == -1)
	{
		ft_putstr_fd("cd: ", 2);
		ft_putstr_fd(path, 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(strerror(errno), 2);
		ft_putstr_fd("\n", 2);
		return (1);
	}
	// Update PWD and OLDPWD environment variables
	pwd = getcwd(NULL, 0);
	printf("Current directory according to getcwd: %s\n", pwd);
	setenv("OLDPWD", oldpwd, 1);
    setenv("PWD", pwd, 1);
    printf("After setenv - PWD=%s\n", getenv("PWD"));
    printf("After setenv - OLDPWD=%s\n", getenv("OLDPWD"));
	free(oldpwd);
	free(pwd);
	return (0);
}