/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_util.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 17:16:28 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/27 17:44:07 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*quote_2_token(char *line, int *l, char c)
{
	char	*str;
	int		i;
	int		k;

	i = 0;
	(*l)++;
	k = *l;
	while (line[*l] && line[*l] != '|' && line[*l] != c)
	{
		i++;
		(*l)++;
	}
	(*l)++;
	str = malloc(sizeof(char) * (i + 1));
	if (!str)
		return (NULL);
	i = 0;
	while (line[k] && line[k] != '|' && line[k] != c)
		str[i++] = line[k++];
	str[i] = '\0';
	return (str);
}
