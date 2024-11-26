/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/01 14:09:24 by adiler            #+#    #+#             */
/*   Updated: 2023/12/11 18:08:24 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	intlen(int n)
{
	int	count;

	count = 0;
	if (n < 0)
	{
		n = -n;
		count++;
	}
	if (n == 0)
		count++;
	while (n != 0)
	{
		n = n / 10;
		count++;
	}
	return (count);
}

static void	fill(long nbr, char *str)
{
	if (nbr >= 10)
		fill(nbr / 10, str - 1);
	*str = nbr % 10 + '0';
}

char	*ft_itoa(int n)
{
	char	*str;
	int		len;
	long	nbr;

	nbr = (long)n;
	len = intlen(nbr);
	str = malloc(len + 1);
	if (!str)
		return (NULL);
	if (nbr < 0)
	{
		str[0] = '-';
		nbr = -nbr;
	}
	fill(nbr, str + len - 1);
	str[len] = 0;
	return (str);
}

// int main()
// {
// 	printf("char %s", ft_itoa(-2147483648));
// }
