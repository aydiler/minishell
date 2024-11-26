/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 19:50:24 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/26 21:56:49 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	parser(char *line, t_cmd *cmd)
{
	int		i;

	printf("cnt: %d\n", count_tokens(line));
	cmd->args = tokenizer(line);
	i = 0;
	while (cmd->args[i])
	{
		printf("%s\n", cmd->args[i]);
		i++;
	}
}
