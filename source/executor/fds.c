#import "../../includes/minishell.h"

int create_empty_files(t_cmd cmd)
{
	char **files = cmd.files_to_create;
	int fd;

	if (!files)
		return 0;
	while (*files)
	{
		fd = open(*files, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
		{
			print_error_message(*files, ERR_PERMISSION);
			return 1;
		}
		close(fd);
		files++;
	}
	return 0;
}

int handle_infile(t_cmd cmd)
{
	int fd = open(cmd.input_file, O_RDONLY);
	if (fd == -1)
	{
		if (errno == ENOENT)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd.input_file, 2);
			ft_putstr_fd(": No such file or directory\n", 2);
		}
		else
			print_error_message(cmd.input_file, ERR_PERMISSION);
		return 1;
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return 1;
	}
	close(fd);
	return 0;
}

int handle_outfile(t_cmd cmd)
{
	int flags = O_WRONLY | O_CREAT;
	if (cmd.append_outfile)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	int fd = open(cmd.output_file, flags, 0644);
	if (fd == -1) {
		print_error_message(cmd.output_file, ERR_PERMISSION);
		return 1;
	}
	if (dup2(fd, STDOUT_FILENO) == -1) {
		close(fd);
		return 1;
	}
	close(fd);
	return 0;
}

void	set_original_fds(t_cmd cmd, int *original_stdout, int *original_stdin)
{
	if (cmd.output_file)
		*original_stdout = dup(STDOUT_FILENO);
	if (cmd.input_file)
		*original_stdin = dup(STDIN_FILENO);
}

void reset_fds(t_cmd cmd, int *original_stdout, int *original_stdin)
{
	if (cmd.output_file)
	{
		dup2(*original_stdout, STDOUT_FILENO);
		close(*original_stdout);
	}
	if (cmd.input_file)
	{
		dup2(*original_stdin, STDIN_FILENO);
		close(*original_stdin);
	}
}