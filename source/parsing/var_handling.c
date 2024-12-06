#include "../../includes/minishell.h"

int	handle_var(t_cmd **cmd, int i)
{
	int	error_check;

	error_check = check_var((*cmd)->args[i]);
	if (error_check)
		return (error_check);
	error_check = check_decl((*cmd)->args[0]);
	if (error_check)
		return (error_check);
	
	access_envp_and_replace_var;
}
