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

#define ERR_NOT_FOUND 1
#define ERR_PERMISSION 2
#define ERR_SYSTEM 3

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
	//parser utils
char	*quote_2_token(char *line, int *l, char c);
t_cmd	*new_pipe(char **args);
int		next_pipe(char *line, int i);
char	**remove_n_token(char **args, int i, int n);
char	**ft_realloc(char **arr, size_t new_size);
	// handle redirections
int		process_redirections(t_cmd **cmd, char **args, char *token, int i);
int		check_redirections(char *token);
	// non args utils
int		handle_var(t_cmd **cmd, int i);
// Executer funktions
int		execute_command(t_cmd cmd, char **envp, void (*signal_handler)(int));
int		print_envp(char **envp);
char	*find_command_in_path(char *cmd);
// utils
void	ft_error(t_cmd *cmd, char *error_message);
void	free_all(t_cmd *cmd);
void	ft_free_split(char **str);
// signals
void	signal_handler(int signo);
void	setup_signals(void);
// tester
void	print_struct(t_cmd *cmd);
void	print_args(char **args);


#endif
