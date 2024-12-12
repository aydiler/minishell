/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strn.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 16:43:50 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/12 16:50:58 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strn(char *str, int n)
{
	char	*new_str;
	int		i;

	if (!str || !n)
		return (NULL);
	new_str = malloc(sizeof(char) * n);
	if (!new_str)
		return (NULL);
	i = 0;
	while (i < n)
	{
		new_str[i] = str[i];
		i++;
	}
	new_str[i] = '\0';
	return (new_str);
}
