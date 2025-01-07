#include "../includes/minishell.h"

volatile sig_atomic_t g_child_running;

int main(int argc, char **argv, char **envp)
{
	char	*input;
	t_cmd	*cmd;
	int		exit_status;
	int		error_check;

	g_child_running = 0;
	error_check = 0;
	cmd = NULL;
	exit_status = 0;
	(void)argc;
	(void)argv;
	setup_parent_signals();
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
		if (input[0])
			error_check = parser(&input, &cmd);
		ft_memdel((void **)&(input));
		if (error_check != ERR_ENV_VAR && (error_check || !cmd))
		{
			ft_error(&cmd, error_check);
			continue ;
		}
		//print_struct(cmd);
		if (cmd)
		{
			exit_status = execute_pipeline(cmd, envp);
		}
		free_all(&cmd);
	}
	rl_clear_history();
	exit(exit_status);
}
