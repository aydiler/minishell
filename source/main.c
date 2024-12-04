/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 21:59:47 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/29 18:04:48 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int main(int argc, char **argv, char **envp)
{
    char    *input;
    t_cmd   *cmd;
    int     exit_status;

    cmd = NULL;
    exit_status = 0;
    (void)argc;
    (void)argv;
    setup_signals();
    load_history();
    while (1)
    {
        input = readline("minishell$ ");
        if (!input)
        {
            ft_putstr_fd("exit\n", 1);
            break;
        }
        if (ft_strncmp(input, "exit", ft_strlen("exit")) == 0)
        {
            free(input);
            break;
        }
        if(*input)
        {
            add_history(input);
            save_history(input);
        }
        parser(input, &cmd);
        if (cmd)
            exit_status = execute_command(cmd->args, envp, signal_handler);
        free(input);
        free_all(&cmd);
    }
    exit(exit_status);
}