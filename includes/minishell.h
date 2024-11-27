/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 21:42:17 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/27 17:51:34 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <sys/wait.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft/libft.h"

typedef struct s_cmd 
{
	char				**args;
	struct s_cmd	*next;
}	t_cmd;

int		count_tokens(char *line);
void	parser(char *line, t_cmd *cmd);
char	**tokenizer(char *line);
int		execute_command(char **args, char **envp);
int		print_envp(char **envp);

#endif
