/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_util.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 17:16:28 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/19 19:37:57 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

// for tokenizer
char	*quote_2_token(char *line, int *l, char c)
{
	char	*str;
	int		i;
	int		k;

	i = 0;
	(*l)++;
	k = *l;
	while (line[*l] && line[*l] != '|' && line[*l] != c)
	{
		i++;
		(*l)++;
	}
	(*l)++;
	str = malloc(sizeof(char) * (i + 1));
	if (!str)
		return (NULL);
	i = 0;
	while (line[k] && line[k] != '|' && line[k] != c)
		str[i++] = line[k++];
	str[i] = '\0';
	return (str);
}

char	**ft_realloc(char **arr, size_t new_size)
{
	char	**new_arr;
	size_t	i;

	i = 0;
	if (!arr)
		return (malloc(sizeof(char *) * new_size));
	if (new_size == 0)
	{
		ft_memdel((void **)&(arr));
		return (NULL);
	}
	new_arr = malloc(sizeof(char *) * new_size);
	if (!new_arr)
		return (NULL);
	while (arr[i] && i < new_size)
	{
		new_arr[i] = arr[i];
		i++;
	}
	ft_memdel((void **)&(arr));
	return (new_arr);
}

char	**remove_n_token(char **args, int i, int n)
{
	int	j;

	if (!args || !args[i] || n <= 0 || !args[i + n - 1])
		return (args);
	while (args[i] && n > 0)
	{
		ft_memdel((void **)&(args[i]));
		j = i;
		while (args[j + 1])
		{
			args[j] = args[j + 1];
			j++;
		}
		args[j] = NULL;
		n--;
	}
	return (args);
}

int	next_pipe(char *line, int i)
{
	while (line[i] && line[i] != '|')
		i++;
	return (i);
}

t_cmd	*new_pipe(char **args)
{
	t_cmd	*pipe;

	pipe = malloc(sizeof(t_cmd));
	if (!pipe)
		return (NULL);
	pipe->args = args;
	pipe->input_file = NULL;
	pipe->output_file = NULL;
	pipe->append_outfile = 0;
	pipe->files_to_create = NULL;
	pipe->next = NULL;
	return (pipe);
}
