/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/01 18:52:12 by adiler            #+#    #+#             */
/*   Updated: 2023/12/08 21:21:32 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	write_n(long nb, int fd)
{
	char	ch;

	if (nb == 0)
		return ;
	write_n(nb / 10, fd);
	ch = nb % 10 + '0';
	write(fd, &ch, 1);
}

void	ft_putnbr_fd(int n, int fd)
{
	long	nb;

	if (n == 0)
	{
		write(fd, "0", 1);
		return ;
	}
	nb = n;
	if (n < 0)
	{
		write(fd, "-", 1);
		nb = -nb;
	}
	write_n(nb, fd);
}
