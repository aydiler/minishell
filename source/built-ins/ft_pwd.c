/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/20 17:36:33 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/23 21:10:00 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int ft_pwd(void)
{
	char buf[PATH_MAX];
	
	if (getcwd(buf, sizeof(buf)))
	{
		ft_putstr_fd(buf, 1);
		ft_putstr_fd("\n", 1);
		return (0);
	}
	return (1);
}