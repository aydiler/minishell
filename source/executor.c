/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 16:58:25 by adiler            #+#    #+#             */
/*   Updated: 2024/11/28 21:04:04 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void ft_free_split(char **str)
{
	int i;

	i = 0;
	while (str[i])
	{
		free(str[i]);
		i++;
	}
	free(str);
}

static char *find_command_in_path(char *cmd)
{
    char *path;
    char **paths;
    char *full_path;
    char *temp;
    
    if (cmd[0] == '/' || cmd[0] == '.')
        return (cmd);
    path = getenv("PATH");
    paths = ft_split(path, ':');
    if (!paths)
        return (NULL);
    for (int i = 0; paths[i]; i++)
    {
        temp = ft_strjoin(paths[i], "/");
        full_path = ft_strjoin(temp, cmd);
        free(temp);
        if (access(full_path, F_OK) == 0)
        {
            ft_free_split(paths);
            return (full_path);
        }
        free(full_path);
    }
    ft_free_split(paths);
    return (NULL);
}

static int handle_child_process(char **args, char **env)
{
    char *cmd_path;
    
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
    cmd_path = find_command_in_path(args[0]);
    if (!cmd_path)
        exit(127);
        
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

static int handle_parent_process(pid_t pid, void (*signal_handler)(int))
{
    int status;

    signal(SIGINT, SIG_IGN);
    waitpid(pid, &status, 0);
    signal(SIGINT, signal_handler);
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        return 128 + WTERMSIG(status);
    return 1;
}

int execute_command(char **args, char **envp, void (*signal_handler)(int))
{
    pid_t pid;
    int exit_status;

    if(!args[0])
        return 0;
    pid = fork();
    if (pid == -1)
        return (perror("fork"), 1);
    if (pid == 0)
        return (handle_child_process(args, envp));
    else
    {
        exit_status = handle_parent_process(pid, signal_handler);
        if (exit_status == 127)
            ft_putstr_fd("command not found\n", 2);
        else if (exit_status == 126)
            ft_putstr_fd("permission denied\n", 2);
        return exit_status;
    }
}