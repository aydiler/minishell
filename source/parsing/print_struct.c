/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_struct.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 22:15:38 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/18 22:18:25 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	print_struct(t_cmd *cmd)
{
	int	i;

	i = 0;
	printf("\n-----------------------\n");
	while (cmd)
	{
		i = 0;
		if (cmd->args && cmd->args[i])
			printf("args:\n");
		while (cmd->args && cmd->args[i])
		{
			printf("\"%s\" ", cmd->args[i]);
			i++;
		}
		printf("\n");
		printf("re: \n");
		while (cmd->re)
		{
			if (cmd->re->file)
				printf("\"%s\", ", cmd->re->file);
			if (cmd->re->type == OUT)
				printf("OUT, append: %d\n", cmd->re->append);
			else
				printf("IN\n",);
			cmd->re = cmd->re->next;
		}
		if (!cmd->next)
			break ;
		printf("\nnext pipe:\n");
		cmd = cmd->next;
	}
	printf("-----------------------\n");
}

void	print_args(char **args)
{
	int	i;

	i = 0;
	printf("args:\n");
	while (args && args[i])
	{
		printf("\"%s\" ", args[i]);
		i++;
	}
	printf("\n");
}
