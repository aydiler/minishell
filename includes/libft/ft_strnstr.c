/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ubuntu <ubuntu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 13:27:38 by adiler            #+#    #+#             */
/*   Updated: 2024/08/19 22:57:08 by ubuntu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	int			bigi;
	size_t		len2;
	const char	*littlestart;

	littlestart = little;
	if (*little == '\0')
		return ((char *)big);
	while (*big && len > 0)
	{
		bigi = 0;
		len2 = len;
		little = littlestart;
		while (*little && *(big + bigi) == *little && len2 > 0)
		{
			bigi++;
			len2--;
			little++;
		}
		if (*little == '\0')
			return ((char *)big);
		big++;
		len--;
	}
	return (NULL);
}
