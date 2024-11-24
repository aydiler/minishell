#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

int	main(int argc, char **argv, char **envp)
{
	char	*cmd;

	(void)argc;
	(void)argv;
	(void)envp;
	while (1)
	{
		cmd = readline("minishell$ ");
		if (!cmd)
			break ;
		if (strcmp(cmd, "exit") == 0)
		{
			free(cmd);
			break ;
		}
		printf("cmd: %s\n", cmd);
		free(cmd);
	}
	exit(EXIT_SUCCESS);
}
