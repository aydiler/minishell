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
			break ;
		}
		if (ft_strncmp(input, "exit", ft_strlen("exit")) == 0)
		{
			free(input);
			break ;
		}
		if (*input)
		{
			add_history(input);
			save_history(input);
		}
		if (parser(input, &cmd) || !cmd)
		{
			ft_error(cmd, NULL);
			free(input);
			continue ;
		}
		//print_struct(cmd);
		if (cmd)
			exit_status = execute_pipeline(cmd, envp, signal_handler);
		free(input);
		free_all(cmd);
	}
	exit(exit_status);
}