/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 20:49:29 by adiler            #+#    #+#             */
/*   Updated: 2024/11/27 20:49:30 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	load_history(void)
{
	char	*str;
	char	*newline;
	int		fd;

	fd = open(".command_history", O_RDONLY);
	if(fd < 0)
	{
		printf("No history file found\n");
		return ;
	}
	while ((str = get_next_line(fd)))
	{
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
	int		fd;

	fd = open(".command_history", O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (fd < 0)
	{
		printf("Error opening history file\n");
		return ;
	}
	write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
	close(fd);
}