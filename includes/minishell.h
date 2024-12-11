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

# define ERR_NOT_FOUND	1
# define ERR_PERMISSION	2
# define ERR_SYSTEM		3
# define ERR_INVAL		128  // Invalid arguments
# define ERR_ACCES		13   // Permission denied
# define ERR_ARGC		129  // Incorrect argument count
# define ERR_FORK		130  // Fork error
# define ERR_EXEC		131  // Exec error
# define ERR_SIGNAL		132  // Signal error
# define ERR_NOMEM		12   // Memory allocation error
# define ERR_PIPE		134  // Pipe or redirection error

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
int		handle_env_vars(t_cmd **cmd);
	//parser utils
char	*quote_2_token(char *line, int *l, char c);
t_cmd	*new_pipe(char **args);
int		next_pipe(char *line, int i);
char	**remove_n_token(char **args, int i, int n);
char	**ft_realloc(char **arr, size_t new_size);
	// handle redirections
int		process_redirections(t_cmd **cmd, char **args, char *token, int i);
int		check_redirections(char *token);
// Executer funktions
int		execute_command(t_cmd cmd, char **envp, void (*signal_handler)(int));
int		print_envp(char **envp);
char	*find_command_in_path(char *cmd);
// utils
void	ft_error(t_cmd *cmd, int exit_status);
void	free_all(t_cmd *cmd);
void	ft_free_split(char **str);
// signals
void	signal_handler(int signo);
void	setup_signals(void);
// tester
void	print_struct(t_cmd *cmd);
void	print_args(char **args);


#endif
