/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 21:27:28 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/24 22:02:36 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	count_tokens(char *line)

char	**tokenizer(char *line)
{
	int		i;
	char	**args;

	if (!line)
		return (NULL);
	args = malloc(sizeof(char *) * count_tokens(line));
	return (args);
}