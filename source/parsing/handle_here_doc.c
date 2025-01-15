#include "../../includes/minishell.h"

char	*do_here_doc(char **args, int i)
{
	char	*tmp;
	char	*extra_input;
	char	*input;

	input = NULL;
	while (1)
	{
		tmp = input;
		extra_input = readline("> ");
		if (!strcmp(extra_input, args[i + 1]))
			break ;
		input = ft_strjoin(tmp, extra_input);
		free(tmp);
		tmp = input;
		input = ft_strjoin(tmp, "\n");
		free(tmp);
		free(extra_input);
	}
	free(extra_input);
	return (input);
}

int	handle_here_doc(t_cmd **cmd, char **args, int i)
{
	int		error_check;
	char	*input;

	error_check = 0;
	if (!args[i + 1])
		return (ERR_INVAL);
	// create
	//open
	input = do_here_doc(args, i);
	//ft_putstr_fd(bla)
	if (cmd)
		printf("%s", input);
	if ((*cmd)->input_file)
		ft_memdel((void **)&((*cmd)->input_file));
	(*cmd)->input_file = ft_strdup(/*name of created file*/);
	if (!(*cmd)->input_file)
		return (ERR_NOMEM);
	(*cmd)->args = remove_n_token((*cmd)->args, i, 2);
	return (error_check);
}
