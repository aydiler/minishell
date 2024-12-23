/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built-ins_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 16:18:06 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/23 16:46:12 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	free_new_envp(char **new_envp, int j)
{
	while (--j >= 0)
		free(new_envp[j]);
	free(new_envp);
	return (ERR_NOMEM);
}
