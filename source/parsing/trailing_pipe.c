/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trailing_pipe.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ubuntu <ubuntu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 22:04:34 by ubuntu            #+#    #+#             */
/*   Updated: 2025/01/21 01:13:16 by ubuntu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int is_heredoc_line(const char *line)
{
    return (strstr(line, "<<") != NULL);
}

int check_trailing_pipe(const char *line)
{
    int i = ft_strlen(line) - 1;

    while (i >= 0 && ft_isspace(line[i]))
        i--;
    
    if (i >= 0 && line[i] == '|' && !is_heredoc_line(line))
        return 1;
    return 0;
}

void handle_pipe_delimiter_signal(int signo)
{
    if (signo == SIGINT)
    {
        g_heredoc_signal = 1;
        write(1, "\n", 1);
        close(STDIN_FILENO);
    }
}

void setup_pipe_delim_signals(void)
{
    struct sigaction sa;
    
    sa.sa_handler = handle_pipe_delimiter_signal;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
}

static void cleanup_and_restore(char *extra_input, int stdin_copy)
{
    free(extra_input);
    dup2(stdin_copy, STDIN_FILENO);
    close(stdin_copy);
    setup_parent_signals();
}

static int handle_signal_or_eof(char *extra_input, int stdin_copy)
{
    if (g_heredoc_signal)
    {
        cleanup_and_restore(extra_input, stdin_copy);
        return (130);
    }
    if (!extra_input)
    {
        dup2(stdin_copy, STDIN_FILENO);
        close(stdin_copy);
        return (ERR_SYNTAX_EOF);
    }
    return (0);
}

static void append_input(char **line, char *extra_input)
{
    char *tmp;

    tmp = *line;
    *line = ft_strjoin(tmp, "\n");
    free(tmp);
    tmp = *line;
    *line = ft_strjoin(tmp, extra_input);
    free(tmp);
    free(extra_input);
}

int handle_trailing_pipe(char **line)
{
    char *extra_input;
    int stdin_copy;
    int status;

    setup_pipe_delim_signals();
    g_heredoc_signal = 0;
    stdin_copy = dup(STDIN_FILENO);
    
    while (1)
    {
        extra_input = readline("> ");
        status = handle_signal_or_eof(extra_input, stdin_copy);
        if (status != 0)
            return (status);
        append_input(line, extra_input);
        if (!check_trailing_pipe(*line))
            break;
    }
    close(stdin_copy);
    setup_parent_signals();
    return (0);
}