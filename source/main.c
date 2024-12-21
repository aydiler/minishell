#include "../includes/minishell.h"

int main(int argc, char **argv, char **envp)
{
	char	*input;
	t_cmd	*cmd;
	int		exit_status;

	cmd = NULL;
	exit_status = 0;
	(void)argc;
	(void)argv;
	setup_signals();
	load_history();
	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
		{
			ft_putstr_fd("exit\n", 1);
			exit(exit_status);
		}
		if (ft_strncmp(input, "exit", ft_strlen("exit")) == 0)
		{
			free(input);
			exit(exit_status);
		}
		if (*input)
		{
			add_history(input);
			save_history(input);
		}
		if (input[0])
			exit_status = parser(&input, &cmd);
		ft_memdel((void **)&(input));
		if (exit_status || !cmd)
		{
			ft_error(&cmd, exit_status);
			continue ;
		}
		//print_struct(cmd);
		if (cmd)
			exit_status = execute_pipeline(cmd, envp, signal_handler);
		free_all(&cmd);
	}
	exit(exit_status);
}
