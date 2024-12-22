/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/20 17:36:33 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/21 20:58:45 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	ft_pwd(void)
{
	char	buf[_PC_PATH_MAX];

	printf("%d\n", _PC_PATH_MAX);
	if (getcwd(buf, sizeof(buf)))
	{
		ft_putstr_fd(buf, 2);
		write(1, "\n", 1);
		return (0);
	}
	else
		return (1);
}

