#include "../../includes/minishell.h"

int ft_echo(char **args)
{
	int i;
	int n_flag;
	i = 1;
	n_flag = 0;
	if (args[1] && ft_strncmp(args[1], "-n", 2) == 0)
	{
		n_flag = 1;
		i++;
	}
	while (args[i])
	{
		ft_putstr_fd(args[i], 1);
		if (args[i + 1])
			ft_putchar_fd(' ', 1);
		i++;
	}
	if (!n_flag)
        ft_putchar_fd('\n', 1);
    return (0);
}