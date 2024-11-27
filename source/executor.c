/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 16:58:25 by adiler            #+#    #+#             */
/*   Updated: 2024/11/27 16:13:03 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static char *find_command_in_path(char *cmd, char **env)
{
	(void)env;
    if (cmd[0] == '/' || cmd[0] == '.')
		return (cmd);
	else
		return (NULL);
}

static int handle_child_process(char **args, char **env)
{
    char *cmd_path;
    
    cmd_path = find_command_in_path(args[0], env);
    if (!cmd_path)
        exit(127);  // Command not found
        
    if (execve(cmd_path, args, env) == -1)
    {
        free(cmd_path);
        if (errno == ENOENT || errno == ENOTDIR || errno == ELOOP)
            exit(127);
        else if (errno == EACCES || errno == EISDIR)
            exit(126);
        exit(1);
    }
    return (0);
}

static int handle_parent_process(pid_t pid)
{
    int status;

    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
    {
        status = WEXITSTATUS(status);
        if (status == 127)
            ft_putstr_fd("command not found\n", 2);
        else if (status == 126)
            ft_putstr_fd("permission denied\n", 2);
        return (status);
    }
    return (0);
}

int execute_command(char **args, char **envp)
{
    pid_t pid;

    pid = fork();
    if (pid == -1)
        return (perror("fork"), 1);
    if (pid == 0)
        return (handle_child_process(args, envp));
    else
        return (handle_parent_process(pid));
}