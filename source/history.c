/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 20:49:29 by adiler            #+#    #+#             */
/*   Updated: 2025/01/21 19:12:58 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	load_history(void)
{
	char	*str;
	char	*newline;
	int		fd;

	fd = open(".command_history", O_RDONLY);
	if (fd < 0)
		return ;
	while (1)
	{
		str = get_next_line(fd);
		if (!str)
			break ;
		newline = ft_strchr(str, '\n');
		if (newline)
			*newline = '\0';
		add_history(str);
		free(str);
	}
	close(fd);
}

void	save_history(char *line)
{
	int			fd;
	static char	history_location[PWD_MAX] = {0};

	if (!history_location[0])
	{
		getcwd(history_location, PWD_MAX);
		ft_strlcat(history_location, "/.command_history", PWD_MAX);
	}
	fd = open(history_location, O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (fd < 0)
	{
		printf("Error opening history file\n");
		return ;
	}
	write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
	close(fd);
}
