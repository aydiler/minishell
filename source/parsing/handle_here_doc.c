#include "../../includes/minishell.h"

static int	process_line(char **input, char *extra_input, char *delimiter)
{
	char	*tmp;

	add_history(extra_input);
	tmp = *input;
	*input = ft_strjoin(tmp, extra_input);
	free(tmp);
	if (!strcmp(extra_input, delimiter))
	{
		free(extra_input);
		return (1);
	}
	tmp = *input;
	*input = ft_strjoin(tmp, "\n");
	free(tmp);
	free(extra_input);
	return (0);
}

void	handle_heredoc_signal(char *input, char *extra_input, int stdin_copy)
{
	free(input);
	free(extra_input);
	dup2(stdin_copy, STDIN_FILENO);
	close(stdin_copy);
}

char	*do_here_doc(char **args, int i)
{
	char	*input;
	char	*extra_input;
	int		stdin_copy;

	stdin_copy = dup(STDIN_FILENO);
	input = ft_strdup("");
	while (1)
	{
		extra_input = readline("> ");
		if (g_heredoc_signal)
		{
			handle_heredoc_signal(input, extra_input, stdin_copy);
			return (NULL);
		}
		if (!extra_input)
		{
			print_here_doc_error(args);
			break ;
		}
		if (process_line(&input, extra_input, args[i + 1]))
			break ;
	}
	close(stdin_copy);
	return (input);
}

int	create_here_doc_file(char *input)
{
	int	fd;

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
	g_heredoc_signal = 0;
	if (!args[i + 1])
		return (ERR_INVAL);
	input = do_here_doc(args, i);
	if (g_heredoc_signal)
	{
		g_heredoc_signal = 0;
		setup_parent_signals();
		return (130);
	}
	error_check = create_here_doc_file(input);
	free(input);
	if ((*cmd)->input_file)
		ft_memdel((void **)&((*cmd)->input_file));
	(*cmd)->input_file = ft_strdup(HEREDOC_FILE);
	if (!(*cmd)->input_file)
		return (ERR_NOMEM);
	(*cmd)->args = remove_n_token((*cmd)->args, i, 2);
	setup_parent_signals();
	return (error_check);
}
