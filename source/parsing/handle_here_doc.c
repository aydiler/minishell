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
	// create history file
	// if (*input)
	// {
	// 	add_history(input);
	// 	save_history(input);
	// }
	free(extra_input);
	return (input);
}

int	create_here_doc_file(char *input)
{
	int		fd;

	fd = open(HEREDOC_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return (ERR_PERMISSION);
	write(fd, input, ft_strlen(input));
	close(fd);
	return (0);
}

int	handle_here_doc(t_cmd **cmd, char **args, int i)
{
	int		error_check;
	char	*input;

	setup_here_doc_signals();
	error_check = 0;
	if (!args[i + 1])
		return (ERR_INVAL);
	input = do_here_doc(args, i);
	error_check = create_here_doc_file(input);
	free(input);
	if ((*cmd)->input_file)
		ft_memdel((void **)&((*cmd)->input_file));
	(*cmd)->input_file = ft_strdup(HEREDOC_FILE);
	if (!(*cmd)->input_file)
		return (ERR_NOMEM);
	(*cmd)->args = remove_n_token((*cmd)->args, i, 2);
	return (error_check);
}
