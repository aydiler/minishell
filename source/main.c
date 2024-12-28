#include "../includes/minishell.h"

int main(int argc, char **argv, char **envp)
{
	char	*input;
	t_cmd	*cmd;
	int		exit_status;
	int		error_check;
	char	**dup_envp;
	int		i;

	error_check = 0;
	dup_envp = ft_2Ddup(envp);
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
		if (input[0])
			error_check = parser(&input, &cmd, dup_envp);
		ft_memdel((void **)&(input));
		if (error_check != ERR_ENV_VAR && (error_check || !cmd))
		{
			ft_error(&cmd, error_check);
			continue ;
		}
		//print_struct(cmd);
		if (cmd)
			exit_status = execute_pipeline(cmd, &dup_envp, signal_handler);
		free_all(&cmd);
	}
	i = 0;
	while(dup_envp[i])
		i++;
	ft_free_arr(dup_envp, i);
	rl_clear_history();
	exit(exit_status);
}
