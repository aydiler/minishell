/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_struct.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 22:15:38 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/19 17:33:34 by maahoff          ###   ########.fr       */
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
		printf("args:\n");
		while (cmd->args && cmd->args[i])
		{
			printf("\"%s\" ", cmd->args[i]);
			i++;
		}
		printf("\n");
		if (cmd->input_file)
			printf("input_file: \"%s\"\n", cmd->input_file);
		if (cmd->output_file)
			printf("output_file: \"%s\"\n", cmd->output_file);
		if (cmd->append_outfile)
			printf("append_outfile: \"%d\"\n", cmd->append_outfile);
		i = 0;
		if (cmd->files_to_create)
		{
			printf("files_to_create:\n");
			while (cmd->files_to_create[i])
			{
				printf("\"%s\" ", cmd->files_to_create[i]);
				i++;
			}
			printf("\n");
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
