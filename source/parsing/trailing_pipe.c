/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trailing_pipe.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 22:04:34 by ubuntu            #+#    #+#             */
/*   Updated: 2025/01/22 19:13:21 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	check_trailing_pipe(const char *line)
{
	int	i;

	i = ft_strlen(line) - 1;
	while (i >= 0 && ft_isspace(line[i]))
		i--;
	if (i >= 0 && line[i] == '|' && !ft_strnstr(line, "<<", ft_strlen(line)))
		return (1);
	return (0);
}

static int	handle_signal_or_eof(char *extra_input, int stdin_copy)
{
	if (g_heredoc_signal)
	{
		free(extra_input);
		dup2(stdin_copy, STDIN_FILENO);
		close(stdin_copy);
		setup_parent_signals();
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

static void	append_input(char **line, char *extra_input)
{
	char	*tmp;

	tmp = *line;
	*line = ft_strjoin(tmp, "\n");
	free(tmp);
	tmp = *line;
	*line = ft_strjoin(tmp, extra_input);
	free(tmp);
	free(extra_input);
}

int	handle_trailing_pipe(char **line)
{
	char	*extra_input;
	int		stdin_copy;
	int		status;

	if (!check_trailing_pipe(*line))
		return (0);
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
			break ;
	}
	close(stdin_copy);
	setup_parent_signals();
	return (0);
}
