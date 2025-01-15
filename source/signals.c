/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/28 20:41:37 by adiler            #+#    #+#             */
/*   Updated: 2025/01/15 20:29:54 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void handle_signal_std(int signo)
{
    if (!g_child_running)
    {
        if (signo == SIGINT)
        {
            write(STDOUT_FILENO, "\n", 1);
            rl_on_new_line();
            rl_replace_line("", 0);
            rl_redisplay();
        }
    }
    else
    {
        if (signo == SIGINT)
            write(1, "\n", 1);
    }
}

void handle_signal_heredoc(int signo)
{
	printf("signo = %d\n", signo);
	if (signo == SIGINT)
	{
		
	}
	if (signo == SIGQUIT)
	{
		
	}
}

void setup_here_doc_signals(void)
{
	signal(SIGINT, handle_signal_heredoc);
	signal(SIGQUIT, handle_signal_heredoc);
}

void setup_parent_signals(void)
{
    signal(SIGINT, handle_signal_std);
    signal(SIGQUIT, SIG_IGN);
}

void setup_child_signals(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}
