/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_struct.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ubuntu <ubuntu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 22:15:38 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/19 20:48:21 by ubuntu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	print_struct(t_cmd *cmd)
{
	int		i;
	t_red	*temp_re;

	i = 0;
	printf("\n=========================\n");
	while (cmd)
	{
		// Print command arguments
		i = 0;
		if (cmd->args && cmd->args[i])
			printf("Command arguments:\n");
		while (cmd->args && cmd->args[i])
		{
			printf("\"%s\" ", cmd->args[i]);
			i++;
		}
		printf("\n");
		// Print redirections
		printf("\nRedirections:\n");
		temp_re = cmd->re;
		if (!temp_re)
			printf("No redirections found\n");
		while (temp_re)
		{
			printf("File: ");
			if (temp_re->file)
				printf("\"%s\"", temp_re->file);
			else
				printf("(null)");
			printf("\nType: %s", temp_re->type == OUT ? "OUTPUT" : "INPUT");
			printf("\nAppend mode: %s", temp_re->append ? "YES" : "NO");
			printf("\nReal: %d", temp_re->real);
			if (temp_re->next)
				printf("\n\n--- Next Redirection ---\n");
			temp_re = temp_re->next;
		}
		if (!cmd->next)
			break ;
		printf("\n\n========= NEXT PIPE =========\n");
		cmd = cmd->next;
	}
	printf("\n=========================\n");
}

void	print_args(char **args)
{
	int	i;

	i = 0;
	printf("Arguments:\n");
	while (args && args[i])
	{
		printf("\"%s\" ", args[i]);
		i++;
	}
	printf("\n");
}

