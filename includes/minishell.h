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
# include <fcntl.h>
# include "libft/libft.h"
# include "../source/helpers/get_next_line/get_next_line.h"
# include <signal.h>
# include <termios.h>

typedef struct s_cmd 
{
	char			**args;
	struct s_cmd	*next;
}	t_cmd;
// Parser funktions:
void	parser(char *line, t_cmd **cmd);
char	**tokenizer(char *line);
char	*quote_2_token(char *line, int *l, char c);
// Executer funktions
int		execute_command(char **args, char **envp, void (*signal_handler)(int));
int		print_envp(char **envp);
// utils
void	free_all(t_cmd **cmd);
// signals
void	signal_handler(int signo);
void	setup_signals(void);

#endif
