/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/28 20:41:37 by adiler            #+#    #+#             */
/*   Updated: 2024/12/10 17:56:48 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	signal_handler(int signo)
{
	if (signo == SIGINT)
	{
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

void    setup_signals(void)
{
    struct sigaction    sa;

    sa.sa_handler = signal_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGQUIT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}