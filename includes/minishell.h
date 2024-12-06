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
	char			*input_file;
	char			*output_file;
	int				append_outfile;
	char			**files_to_create;
	struct s_cmd	*next;
}	t_cmd;
// Parser funktions:
int		parser(char *line, t_cmd **cmd);
char	**tokenizer(char *line);
char	*quote_2_token(char *line, int *l, char c);
	// handle non_args
char	**remove_token(char **args, int i);
char	**remove_n_token(char **args, int i, int n);
int		check_non_args(char *str);
int		handle_re_output(t_cmd **cmd, char **args, int i);
int		handle_re_input(t_cmd **cmd, char **args, int i);
int		handle_ap_output(t_cmd **cmd, char **args, int i);
char	**ft_realloc(char **arr, size_t new_size);
// Executer funktions
int		execute_command(char **args, char **envp, void (*signal_handler)(int));
int		print_envp(char **envp);
// utils
void	ft_error(t_cmd *cmd, char *error_message);
void	free_all(t_cmd *cmd);
// signals
void	signal_handler(int signo);
void	setup_signals(void);
// tester
void	print_struct(t_cmd *cmd);

#endif
