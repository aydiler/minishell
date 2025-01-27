/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 12:42:03 by maweiss           #+#    #+#             */
/*   Updated: 2024/10/30 12:12:48 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* General header file for minishell. All subsequent header files are included
in minishell.h */

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
# include <limits.h>
# include <stdbool.h>
# include <signal.h>
# include "../libft/libft.h"
# include "ms_symtab.h"
# include "ms_parsing.h"
# include "ms_garbage.h"
# include "ms_executor.h"
# include "tokenization.h"
# include "../libft/libft.h"


#define FALSE 0
#define TRUE 1

/* minishell struct. Main struct that is passed throughout the whole program.
	global flags: 1 = heredoc present	*/
typedef struct s_ms {
	char			*cmd;
	t_cmd_list		*cmds;
	int				global_flags;
	t_be			*be;
}				t_ms;

/* ms_cleanup_utils */

void			ft_clear_be(t_ms *ms);
void			ft_clean_be(t_ms *ms);
void			ft_clear_ast(t_ms *ms);
void			clean_garbage(void);
void			ft_lsthdgbdelone(t_list_hdfiles *lst, void (*del)(void *));
void			ft_delfree_hdgb(t_list_hdfiles **lst, void (*del)(void *));
void			ft_cleanup_exit(t_ms *ms, int ex);

/* ms_executor */

void			ft_close_all_fds(t_ms *ms);
char			*ft_search_cmd(t_ms *ms, t_cmd_list *curr);
void			ft_execute(t_ms *ms, t_cmd_list *curr);
int				ft_builtin(t_ms *ms, t_cmd_list *curr);
void			ft_create_argv(t_cmd_list *curr);
void			ft_fork_execute(t_ms *ms, t_cmd_list *curr, int *i);
void			ft_is_builtin(t_cmd_list *curr, t_ms *ms);
void			ft_executor(t_ms *ms);
void			ft_back_end(t_ms *ms);

/* ms_error */

void			ft_wait_error(t_ms *ms);

/* ms_ex_redir */

void			ft_outpipe(t_ms *ms, int i);
void			ft_outfile(t_ms *ms, char *filename, int mode);
void			ft_infile(t_ms *ms, char *filename);
void			ft_inpipe(t_ms *ms, int i);
void			ft_redir_handler(t_ms *ms, t_cmd_list *curr, int i);
void			ft_ex_prio(t_cmd_list *curr);

/* ms_heredoc */

char			*ft_search_tmp(void);
void			ft_garbage_add(char *filename, t_ms *ms);
char			*ft_tmp_name(t_ms *ms, int *fd);
void			ft_hd_input(t_list_redir *curr, t_ms *ms);
void			ft_here_doc(t_ms *ms);

/* ms_init */

char			**ft_grab_envp(char **envp);
void			ft_init_ms(t_ms *ms);
void			ft_reinit_be(t_ms *ms);
void			ft_init_be(t_ms *ms, int argc, char **argv, char **envp);

/* ms_ui */

void			ft_front_end(char *cmd);
char			*choose_prompt(int mode);
void			ft_repl(int argc, char **argv, char **envp);

/* ms_debug */

void			ft_deb_here_doc(t_ms *ms);
void			ft_deb_commands(t_ms *ms);
void			ft_debug(t_ms *ms);

/* ms_env */

void			ft_init_symtab(t_ms *ms);
void			ft_free_symtab_stack(t_symtab_stack *symtab_stack);
void			ft_add_global_value(t_ms *ms, char *env);
void			ft_add_local_value(t_ms *ms, char *env);
int				ft_remove_from_symtab(t_symtab_stack *symtab_lvl, char *key);
char			*ft_lookup_symtab(t_symtab_stack *symtab_lvl, char *key);
int				ft_update_symtab_value(t_symtab_stack *symtab_lvl, char *key, char *value);
int				ft_make_global(t_ms *ms, char *key);
char			**ft_update_envp_runtime(char **envp, char *key, char *value);
char			**ft_create_envp(t_ms *ms);
void			ft_print_symtab(t_ms *ms, int lvl);
unsigned long	ft_hash_function(t_symtab_stack *current_symtab, char *key);
void			ft_resize_symtab(t_symtab_stack **symtab_lvl);
void			ft_add_to_symtab(t_symtab_stack *symtab_lvl, char *key, char *value);
void			ft_add_local_symtab(t_ms *ms);
int				ft_calc_symtab_size(int size);
int				ft_is_prime(int n);
void			ft_add_value(t_ms *ms, char *env);

/* ms_builtins*/

void			ft_env(t_ms *ms, t_cmd_list *curr);
void			ft_export(t_ms *ms, t_cmd_list *curr);
void			ft_unset(t_ms *ms, t_cmd_list *curr);
void			ft_cd(t_ms *ms, t_cmd_list *curr);
void			ft_pwd(t_ms *ms, t_cmd_list *curr);
int				ft_echo(t_ms *ms, t_cmd_list *curr);

/* ms_parse */
int parse(t_token *tokens);
t_list_words *make_word(t_tok_span *word_info);
t_list_words **make_word_list(t_tok_span *word_info, t_list_words ***tail);

#endif
