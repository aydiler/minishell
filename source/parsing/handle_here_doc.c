#include "../../includes/minishell.h"

char *do_here_doc(char **args, int i)
{
    char    buffer[1024];
    char    *tmp;
    char    *input;
    ssize_t bytes_read;

    input = ft_strdup("");
    write(1, "> ", 2);
    while (1)
    {
        bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        
        if (g_heredoc_signal || bytes_read == -1)
        {
            free(input);
            return (NULL);
        }
        if (bytes_read == 0)
        {
			write(1, "\n", 2);
            ft_putstr_fd("minishell: warning: here-document delimited by end-of-file (wanted `", 2);
            ft_putstr_fd(args[i + 1], 2);
            ft_putstr_fd("')\n", 2);
            break;
        }
        buffer[bytes_read] = '\0';
        tmp = input;
        input = ft_strjoin(tmp, buffer);
        free(tmp);
        if (ft_strnstr(buffer, args[i + 1], bytes_read) && 
            buffer[ft_strlen(args[i + 1])] == '\n')
            break;
            
        write(1, "> ", 2);
    }
    
    return input;
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
	g_heredoc_signal = 0;
	if (!args[i + 1])
		return (ERR_INVAL);
	input = do_here_doc(args, i);
	if (g_heredoc_signal)
	{
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
