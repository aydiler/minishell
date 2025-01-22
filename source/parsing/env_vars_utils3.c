/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars_utils3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 18:32:36 by adiler            #+#    #+#             */
/*   Updated: 2025/01/22 18:42:21 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	check_in_double_quote(char *line, int i)
{
	int	j;

	j = i + 1;
	while (line[j] && line[j] != '\"')
		j++;
	if (!line[j])
		return (1);
	return (0);
}
