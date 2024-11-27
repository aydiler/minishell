/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_cat.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 16:40:29 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/27 17:44:20 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*token_cat(char *token, int *pos, char *str)
{
	int	j;

	j = 0;
	while (str[j] != '\0')
	{
		token[*pos] = str[j];
		(*pos)++;
		j++;
	}
	free(str);
	return (token);
}
