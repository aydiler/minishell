/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_executor.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/26 18:15:36 by maweiss           #+#    #+#             */
/*   Updated: 2024/10/31 12:51:14 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/minishell.h"

// char *ft_complete_path(t_ms *ms, t_cmd_list *curr, char *input)
// {
// 	// if(input[0] == '.' && input[1] == '/')
// 	// 	return ()
// }

void	ft_close_all_fds(t_ms *ms)
{
	if (close(ms->be->pipes[0][0]) || close(ms->be->pipes[0][1])
		|| close(ms->be->pipes[1][0]) || close(ms->be->pipes[1][1]))
	{
		perror("minishell");
	}
}

char	*ft_search_cmd(t_ms *ms, t_cmd_list *curr)
{
	int		i;
	char	*path;

	i = 0;

	// if ((curr->cmd->flags & WORD_IS_PATH) == WORD_IS_PATH)
	// {
	// 	path = ft_complete_path(ms, curr, curr->cmd->words->word);
	// 	return (path);
	// }
	while (ms->be->path[i])
	{
		path = ft_strjoin(ms->be->path[i], curr->cmd->words->word);
		if (path == NULL)
		{
			perror("malloc fail!\n");
			ft_cleanup_exit(ms, ENOMEM);
		}
		if (access(path, X_OK) == 0)
			return (path);
		free(path);
		path = NULL;
		i++;
	}
	return (NULL);
}

void	ft_execute(t_ms *ms, t_cmd_list *curr)
{
	int		err;
	char	*cmdpath;
	char	**envp;

	cmdpath = ft_search_cmd(ms, curr);
	if (cmdpath == NULL)
		err = 127;
	else
	{
		envp = ft_create_envp(ms);
		err = execve(cmdpath, curr->cmd->argv, envp);
		ft_free_2d(envp);
	}
	ft_clear_ast(ms); // [ ] take care of this in case of not a child!!
	ft_clear_be(ms); // [ ] take care of this in case of not a child!!
	ft_cleanup_exit(ms, err);
}

int	ft_builtin(t_ms *ms, t_cmd_list *curr)
{
	int		ret;

	ret = 0;
	if (curr->cmd->builtin_nr == 1)
		ret = ft_echo(ms, curr);
	// else if (curr->cmd->builtin_nr == 2)
	// 	ret = ft_cd(ms, curr);
	// else if (curr->cmd->builtin_nr == 3)
	// 	ret = ft_pwd(ms, curr);
	// else if (curr->cmd->builtin_nr == 4)
	// 	ret = ft_pwd(ms, curr);
	// else if (curr->cmd->builtin_nr == 5)
	// 	ret = ft_export(ms, curr);
	// else if (curr->cmd->builtin_nr == 6)
	// 	ret = ft_env(ms, curr);
	// else if (curr->cmd->builtin_nr == 7)
	// 	ret = ft_exit(ms, curr);
	return (ret);
}

void	ft_create_argv(t_cmd_list *curr)
{
	t_list_words	*words;
	char			**argv;
	int				i;

	words = curr->cmd->words;
	i = 0;
	while (words)
	{
		i++;
		words = words->next;
	}
	argv = ft_calloc(sizeof(char *), i + 1);
	words = curr->cmd->words;
	i = 0;
	while (words)
	{
		argv[i++] = ft_strdup(words->word);
		words = words->next;
	}
	curr->cmd->argv = argv;
}


void	ft_fork_execute(t_ms *ms, t_cmd_list *curr, int *i)
{

	if ((curr->cmd->flags & WORD_IS_BUILTIN) != WORD_IS_BUILTIN || ms->be->nb_cmds > 1)
		ms->be->child_pids[*i] = fork();
	else
		ms->be->child_pids[*i] = INT_MAX;
	if (ms->be->child_pids[*i] < 0)
	{
		strerror(EPIPE);
		ft_cleanup_exit(ms, EPIPE);
	}
	if (ms->be->child_pids[*i] == 0 || ms->be->child_pids[*i] == INT_MAX)
	{
		ft_redir_handler(ms, curr, *i);
		ft_create_argv(curr);
		if(ms->be->child_pids[*i] != INT_MAX)
			ft_close_all_fds(ms);
		if (ms->be->child_pids[*i] == INT_MAX)
			ft_builtin(ms, curr);
		else if (ms->be->child_pids[*i] == 0)
			ft_execute(ms, curr);
	}
}

void	ft_is_builtin(t_cmd_list *curr, t_ms *ms)
{
	int		i;
	int		len;

	i = -1;
	while (ms->cmds->cmd->words && ms->be->builtins[++i])
	{
		len = ft_strlen(ms->be->builtins[i]);
		if (strncmp(curr->cmd->words->word, ms->be->builtins[i], len) == 0
			&& (int) ft_strlen(curr->cmd->words->word) == len)
		{
			curr->cmd->flags |= WORD_IS_BUILTIN;
			curr->cmd->builtin_nr = i + 1;
		}
	}
}

void	ft_executor(t_ms *ms)
{
	t_cmd_list			*curr;
	int					i;

	curr = ms->cmds;
	i = 0;
	while (curr)
	{
		ft_is_builtin(curr, ms);
		if (curr->cmd->redir)
		{
			ft_ex_prio(curr);
		}
		ft_fork_execute(ms, curr, &i);
		curr = curr->next;
		i++;
	}
}

void	ft_back_end(t_ms *ms)
{
	ft_reinit_be(ms);
	if (ms->global_flags == 1)
		ft_here_doc(ms);
	if(ms->cmds->cmd)
	{
		ft_executor(ms);
		ft_close_all_fds(ms);
		ft_wait_error(ms);
		ft_clear_ast(ms);
	}
	ft_clear_be(ms);
}
