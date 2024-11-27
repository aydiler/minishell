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
	char			**args;
	struct s_cmd	*next;
}	t_cmd;
// Parser funktions:
void	parser(char *line, t_cmd *cmd);
char	**tokenizer(char *line);
int		execute_command(char **args, char **envp);
int		print_envp(char **envp);
char	*quote_2_token(char *line, int *l, char c);
int		count_tokens(char *line);

#endif
