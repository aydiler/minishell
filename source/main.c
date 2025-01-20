#include "../includes/minishell.h"

volatile sig_atomic_t	g_child_running;
volatile sig_atomic_t	g_heredoc_signal;

int	main(int argc, char **argv, char **envp)
{
	char *input;
	t_cmd *cmd;
	int exit_status;
	int error_check;
	char **dup_envp;

	g_child_running = 0;
	g_heredoc_signal = 0;
	error_check = 0;
	dup_envp = ft_arrdup(envp);
	cmd = NULL;
	exit_status = 0;
	setup_parent_signals();

	// Check for -c flag
	if (argc >= 3 && ft_strcmp(argv[1], "-c") == 0)
	{
		input = ft_strdup(argv[2]);
		if (input[0])
			error_check = parser(&input, &cmd, dup_envp, exit_status);
		ft_memdel((void **)&(input));
		if (error_check || !cmd)
		{
			ft_error(&cmd, error_check);
			exit_status = 1;
		}
		else if (cmd)
		{
			exit_status = execute_pipeline(cmd, &dup_envp);
			free_all(&cmd);
		}
		if (dup_envp)
			ft_free_arr(&dup_envp);
		exit(exit_status);
	}

	// Normal interactive mode
	load_history();
	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
		{
			ft_putstr_fd("exit\n", 1);
			break ;
		}
		if (*input)
		{
			add_history(input);
			save_history(input);
		}
		if (input[0])
			error_check = parser(&input, &cmd, dup_envp, exit_status);
		ft_memdel((void **)&(input));
		if (error_check || !cmd)
		{
			ft_error(&cmd, error_check);
			continue ;
		}
		print_struct(cmd);
		if (cmd)
			exit_status = execute_pipeline(cmd, &dup_envp);
		free_all(&cmd);
	}
	if (dup_envp)
		ft_free_arr(&dup_envp);
	rl_clear_history();
	exit(exit_status);
}