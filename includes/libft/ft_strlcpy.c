/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/16 20:47:24 by adiler            #+#    #+#             */
/*   Updated: 2023/12/11 17:32:18 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t		len;

	len = ft_strlen(src);
	if (size == 0)
		return (len);
	while (*src && size-- > 1)
		*(dst++) = *(src++);
	*dst = 0;
	return (len);
}
