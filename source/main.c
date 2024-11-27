/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 21:59:47 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/27 16:17:00 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	main(int argc, char **argv, char **envp)
{
	char	*input;
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (1);
	(void)argc;
	(void)argv;
	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
			break ;
		if (strcmp(input, "exit") == 0)
		{
			free(input);
			break ;
		}
		parser(input, cmd);
		print_envp(envp);
		execute_command(cmd->args, envp);
		free(input);
	}
	exit(EXIT_SUCCESS);
}
