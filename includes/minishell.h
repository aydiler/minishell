/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 15:59:32 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/22 20:43:59 by adiler           ###   ########.fr       */
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
# include <fcntl.h>
# include "libft/libft.h"
# include "../source/helpers/get_next_line/get_next_line.h"
# include <signal.h>
# include <termios.h>
# include <limits.h>
# include <sys/ioctl.h>

# define ERR_NOT_FOUND	1
# define ERR_PERMISSION	2
# define ERR_SYSTEM		3
# define ERR_INVAL		128		// Invalid arguments
# define ERR_ACCES		13		// Permission denied
# define ERR_ARGC		129		// Incorrect argument count
# define ERR_FORK		130		// Fork error
# define ERR_EXEC		131		// Exec error
# define ERR_SIGNAL		132		// Signal error
# define ERR_NOMEM		12		// Memory allocation error
# define ERR_PIPE		134		// Pipe or redirection error
# define ERR_UNMATCHED	99		// Unmatched quotations
# define ERR_SIGINT		130		// Ctrl + C
# define ERR_SYNTAX		258		// Syntax error
# define ERR_SYNTAX_EOF	259		// Syntax error
# define ERR_FILE		127	// File not found
# define NOVAR			-1		// empty line
# define PWD_MAX		4096
# define READ_END		0
# define WRITE_END		1
# define HEREDOC_FILE	"/tmp/.minishell_here_doc"
# define CMD_HISTORY	"/tmp/.command_history"
# define OUT			1
# define IN				2

extern volatile sig_atomic_t	g_child_running;
extern volatile sig_atomic_t	g_heredoc_signal;

typedef struct s_red
{
	char			*file;
	int				type;
	int				append;
	int				real;
	struct s_red	*next;
}	t_red;

typedef struct s_cmd 
{
	char			**args;
	struct s_red	*re;
	struct s_cmd	*next;
	int				outfile;
	int				infile;
}	t_cmd;
// Parser funktions:
int		parser(char **line, t_cmd **cmd, char **envp, int exit_status);
char	**tokenizer(char *line);
int		handle_empty_pipe(char *line);
int		handle_env_vars(char **line, char **envp, int exit_status);
int		handle_unclosed_quotes(char **line);
int		handle_here_doc(t_cmd **cmd, char **args, int i);
int		handle_trailing_pipe(char **line);
int		check_trailing_pipe(const char *line);
	//parser utils
int		jump_quote(const char *line, int index);
char	*quote_2_token(char *line, int *l, char c);
t_cmd	*new_pipe(char **args);
int		next_pipe(char *line, int i);
char	**remove_n_token(char **args, int i, int n);
char	**ft_realloc(char **arr, size_t new_size);
int		jump_s_quote(char *line, int start);
void	quote_handling(char *line, int *i, int *j, char c);
	// env_vars utils
int		check_in_double_quote(char *line, int i);
int		is_env_var(char c);
int		has_env_var(char *line);
int		is_exit_status_var(char *line);
int		is_tilde(char *line, int i);
int		exchange_tilde(char **line, int i);
int		skip_quote(char *line, int *i);
char	*getenv_range(char *start, size_t *len_var, char **envp);
int		fill_in_exit_status(char **line, int exit_status);
int		remove_false_var(char **line, int var_check);
int		find_var_start(char *line);
	// handle redirections
t_red	*new_re(void);
int		prep_redirections(char **line);
int		process_redirections(t_cmd **cmd, char **args, char *token, int i);
int		check_redirections(char *token);
void	handle_redirection_execution(t_cmd cmd);
	// non args utils
int		handle_var(t_cmd **cmd, int i);
// Executer functions
int		execute_pipeline(t_cmd *cmd, char ***envp);
int		print_envp(char **envp);
char	*find_command_in_path(char *cmd, char **envp);
void	free_executor(int **pipes, int cmd_count, int *pids);
// utils
void	ft_error(t_cmd **cmd, int exit_status);
void	free_all(t_cmd **cmd);
void	ft_free_split(char **str);
char	**ft_arrdup(char **arr);
char	*ft_getenv(char *name, char **envp);
int		is_valid_number(char *str, int sign);
int		increase_shlvl(char ***dup_envp);
// signals
void	signal_handler(int signo);
void	setup_signals(void);
void	setup_signal(int signo, void (*handler)(int));
void	handle_signal_std(int signo);
void	handle_signal_std(int signo);
void	setup_signal(int signo, void (*handler)(int));
void	setup_parent_signals(void);
void	setup_child_signals(void);
void	setup_here_doc_signals(void);
void	handle_pipe_delimiter_signal(int signo);
void	setup_pipe_delim_signals(void);
// tester
void	print_struct(t_cmd *cmd);
void	print_args(char **args);
int		print_env(char **envp);
// fds
int		create_empty_files(t_cmd cmd);
int		handle_infile(t_cmd cmd);
int		handle_outfile(t_cmd cmd);
// executor error handling
void	print_error_message(char *cmd, int error_type);
void	print_here_doc_error(char **args);
// build-ins
int		print_export_error(char *name);
int		is_valid_env_name(char *name);
char	**ft_sort_envp(char **envp);
int		ft_free_arr(char ***arr);
int		ft_pwd(void);
int		ft_env(char **envp);
int		ft_export(char ***envp, char **args);
int		ft_unset(char ***envp, char **args);
int		ft_echo(char **args);
int		ft_cd(char **args);
int		ft_exit(char **args);
// pipes
int		count_pipes(t_cmd *cmd);
void	free_pipes(int **pipes, int cmd_count);
int		**create_pipe_array(int cmd_count);
int		create_pipes(int **pipes, int cmd_count);
void	close_pipes(int **pipes, int cmd_count);
int		initialize_pipeline(t_cmd *cmd, int ***pipes, int **pids);
void	setup_pipe_fds(t_cmd *cmd, int **pipes, int cmd_index, int cmd_count);

// builtins
int		is_child_builtin(char **args);
int		is_parent_builtin(char **args);
void	execute_child_builtin(char **args, char ***envp);
int		execute_parent_builtin(t_cmd *cmd, char ***envp);
int		ft_cd(char **args);

#endif
