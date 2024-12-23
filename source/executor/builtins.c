/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 21:05:20 by adiler            #+#    #+#             */
/*   Updated: 2024/12/22 21:20:27 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int		is_child_builtin(char **args)
{
	(void)args;
	// if (!ft_strncmp(args[0], "echo", 5))
	// 	return (1);
	// if (!ft_strncmp(args[0], "pwd", 4))
	// 	return (1);
	// if (!ft_strncmp(args[0], "env", 4))
	// 	return (1);
	return (0);
}

void	handle_child_builtin(char **args)
{
	int		exit_status;

	(void)args;
	exit_status = 0;
	// if (!ft_strncmp(args[0], "echo", 5))
	// 	exit_status = builtin_echo(args);
	// if (!ft_strncmp(args[0], "pwd", 4))
	// 	exit_status = builtin_pwd();
	// if (!ft_strncmp(args[0], "env", 4))
	// 	exit_status = print_envp(args);
	exit(exit_status);
}


int	is_parent_builtin(char **args)
{
	if (!ft_strncmp(args[0], "cd", 3))
		return (1);
	// if (!ft_strncmp(args[0], "export", 7))
	// 	return (1);
	// if (!ft_strncmp(args[0], "unset", 6))
	// 	return (1);
	// if (!ft_strncmp(args[0], "exit", 5))
	// 	return (1);
	return (0);
}

int execute_parent_builtin(t_cmd *cmd)
{
	int exit_status;
	int stdin_backup = dup(STDIN_FILENO);
	int stdout_backup = dup(STDOUT_FILENO);

	handle_redirection_execution(*cmd);
	
	if (!ft_strncmp(cmd->args[0], "cd", 3))
		exit_status = builtin_cd(cmd->args);
	// if (!ft_strncmp(cmd->args[0], "export", 7))
	// 	exit_status = builtin_export(cmd->args);
	// if (!ft_strncmp(cmd->args[0], "unset", 6))
	// 	exit_status = builtin_unset(cmd->args);
	// if (!ft_strncmp(cmd->args[0], "exit", 5))
	// 	exit_status = builtin_exit(cmd->args);
	
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
	
	return exit_status;
}