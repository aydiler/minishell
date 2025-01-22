/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trailing_pipe_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 19:16:27 by adiler            #+#    #+#             */
/*   Updated: 2025/01/21 19:29:00 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	handle_pipe_delimiter_signal(int signo)
{
	if (signo == SIGINT)
	{
		g_heredoc_signal = 1;
		write(1, "\n", 1);
		close(STDIN_FILENO);
	}
}

void	setup_pipe_delim_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_pipe_delimiter_signal;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
}
