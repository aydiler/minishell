/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ubuntu <ubuntu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/24 23:29:07 by adiler            #+#    #+#             */
/*   Updated: 2025/01/20 18:04:05 by ubuntu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int is_numeric(char *str)
{
    int     i;
    int     len;
    int     sign;

    i = 0;
    len = 0;
    sign = 1;
    if (str[i] == '-' || str[i] == '+')
    {
        if (str[i] == '-')
            sign = -1;
        i++;
    }
    len = ft_strlen(str + i);
    if (len > 19)
        return (0);
    if (len == 19 && !is_valid_number(str + i, sign))
        return (0);
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return (0);
        i++;
    }
    return (1);
}

static long	ft_atol(char *str)
{
	long	num;
	int		sign;
	int		i;

	num = 0;
	sign = 1;
	i = 0;
	if (str[i] == '-')
	{
		sign = -1;
		i++;
	}
	else if (str[i] == '+')
		i++;
	while (str[i])
	{
		num = num * 10 + (str[i] - '0');
		i++;
	}
	return (num * sign);
}

void	err_numeric(char *arg)
{
	ft_putstr_fd("minishell: exit: ", 2);
	ft_putstr_fd(arg, 2);
	ft_putendl_fd(": numeric argument required", 2);
}

int	validate_exit_args(char **args, long *exit_code)
{
	if (!args || !args[1])
		return (0);
	if (!is_numeric(args[1]))
	{
		err_numeric(args[1]);
		exit(2);
	}
	if (args[2])
	{
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		return (1);
	}
	*exit_code = ft_atol(args[1]);
	*exit_code %= 256;
	if (*exit_code < 0)
		*exit_code += 256;
	return (0);
}

int ft_exit(char **args)
{
	long	exit_code;
	int		is_piped;

	exit_code = 0;
	is_piped = !isatty(STDOUT_FILENO);
	if (!is_piped)
		ft_putendl_fd("exit", 1);
	if (validate_exit_args(args, &exit_code))
		return (1);
	exit(exit_code);
	return (0);
}