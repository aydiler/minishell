/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/24 23:29:07 by adiler            #+#    #+#             */
/*   Updated: 2025/01/07 16:29:19 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

// exit too many arguments
// exit in pipe
// free
// exit val not digit
// exit val above below int max
// exit VAL 256 IS MODULO 256

int	ft_exit(char **args)
{
	(void)args;
	printf("exit\n");
	exit(0);
}
