/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_error.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ubuntu <ubuntu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 21:58:23 by ubuntu            #+#    #+#             */
/*   Updated: 2025/01/17 00:43:39 by ubuntu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void    print_path_error(char *cmd)
{
    char    *error_msg;

    error_msg = ft_strjoin("minishell: ", cmd);
    if (error_msg)
    {
        ft_putstr_fd(error_msg, 2);
        if (access(cmd, F_OK) == 0)
        {
            if (access(cmd, X_OK) == 0)
                ft_putstr_fd(": Is a directory\n", 2);
            else
                ft_putstr_fd(": Permission denied\n", 2);
        }
        else
            ft_putstr_fd(": No such file or directory\n", 2);
        free(error_msg);
    }
}

void    print_error_message(char *cmd, int error_type)
{
    if (cmd[0] == '/' || (cmd[0] == '.' && cmd[1] == '/') || 
        (cmd[0] == '.' && cmd[1] == '.' && cmd[2] == '/'))
    {
        print_path_error(cmd);
        return;
    }
    ft_putstr_fd(cmd, 2);
    if (error_type == ERR_NOT_FOUND)
        ft_putstr_fd(": command not found\n", 2);
    else if (error_type == ERR_PERMISSION)
        ft_putstr_fd(": Permission denied\n", 2);
}