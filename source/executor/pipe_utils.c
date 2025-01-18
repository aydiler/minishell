/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 21:55:38 by ubuntu            #+#    #+#             */
/*   Updated: 2025/01/18 21:24:58 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	count_pipes(t_cmd *cmd)
{
	int	count;

	count = 0;
	while (cmd)
	{
		count++;
		//printf("count: %d\n", count);
		cmd = cmd->next;
	}
	return (count);
}
