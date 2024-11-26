/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 12:26:03 by adiler            #+#    #+#             */
/*   Updated: 2023/12/11 17:18:15 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*chr;

	chr = s;
	while (n > 0)
	{
		if (*chr == (unsigned char)c)
			return ((void *)chr);
		chr++;
		n--;
	}
	return (NULL);
}
