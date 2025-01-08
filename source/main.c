#include "../includes/minishell.h"

volatile sig_atomic_t g_child_running;

int main(int argc, char **argv, char **envp)
{
	char	*input;
	t_cmd	*cmd;
	int		exit_status;
	int		error_check;
	char	**dup_envp;

	g_child_running = 0;
	error_check = 0;
	dup_envp = ft_2Ddup(envp);
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
		if (*input)
		{
			add_history(input);
			save_history(input);
		}
		if (input[0])
			error_check = parser(&input, &cmd, dup_envp);
		ft_memdel((void **)&(input));
		if (error_check || !cmd)
		{
			ft_error(&cmd, error_check);
			continue ;
		}
		//print_struct(cmd);
		if (cmd)
			exit_status = execute_pipeline(cmd, &dup_envp);
		free_all(&cmd);
	}
	//printf("len of dup envp: %d\n", ft_arrlen(dup_envp));
	if (dup_envp)
		ft_free_arr(&dup_envp);
	rl_clear_history();
	//printf("exit status: %d\n", exit_status);
	exit(exit_status);
}
