/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_error.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ubuntu <ubuntu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 21:58:23 by ubuntu            #+#    #+#             */
/*   Updated: 2025/01/08 21:59:10 by ubuntu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	print_error_message(char *cmd, int error_type)
{
	char	*error_msg;

	if (cmd[0] == '/' || (cmd[0] == '.' && cmd[1] == '/') || (cmd[0] == '.'
			&& cmd[1] == '.' && cmd[2] == '/'))
	{
		error_msg = ft_strjoin("minishell: ", cmd);
		perror(error_msg);
		free(error_msg);
		return ;
	}
	ft_putstr_fd(cmd, 2);
	if (error_type == ERR_NOT_FOUND)
		ft_putstr_fd(": command not found\n", 2);
	else if (error_type == ERR_PERMISSION)
		ft_putstr_fd(": Permission denied\n", 2);
}
