/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 21:59:47 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/24 22:01:56 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	main(int argc, char **argv, char **envp)
{
	char	*cmd;

	(void)argc;
	(void)argv;
	(void)envp;
	while (1)
	{
		cmd = readline("minishell$ ");
		if (!cmd)
			break ;
		if (strcmp(cmd, "exit") == 0)
		{
			free(cmd);
			break ;
		}
		printf("cmd: %s\n", cmd);
		free(cmd);
	}
	exit(EXIT_SUCCESS);
}
