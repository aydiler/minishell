/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   non_args_util.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 18:35:37 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/29 21:58:20 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	handle_re_output(t_cmd **cmd, char **args, int i)
{
	int	j;

	j = 0;
	if (!args[i + 1])
		return (1);
	if ((*cmd)->output_file)
	{
		if (!(*cmd)->files_to_create)
		{
			(*cmd)->files_to_create = malloc(sizeof(char *));
			if (!(*cmd)->files_to_create)
				return (1); // diffrent error check
		}
		while ((*cmd)->files_to_create[j])
			j++;
		(*cmd)->files_to_create = ft_realloc((*cmd)->files_to_create, (j + 2));
		if (!(*cmd)->files_to_create)
			return (1);
		(*cmd)->files_to_create[j] = ft_strdup((*cmd)->output_file);
		free((*cmd)->output_file);
	}
	(*cmd)->output_file = ft_strdup(args[i + 1]);
	(*cmd)->args = remove_n_token((*cmd)->args, i, 2);
	return (0);
}

int	handle_re_input(t_cmd **cmd, char **args, int i)
{
	if (!args[i + 1])
		return (1);
	(*cmd)->input_file = ft_strdup(args[i + 1]);
	(*cmd)->args = remove_token((*cmd)->args, i);
	(*cmd)->args = remove_token((*cmd)->args, i);
	return (0);
}

int	handle_ap_output(t_cmd **cmd, char **args, int i)
{
	int	j;

	j = 0;
	if (!args[i + 1])
		return (1);
	if ((*cmd)->output_file)
	{
		if (!(*cmd)->files_to_create)
		{
			(*cmd)->files_to_create = malloc(sizeof(char *));
			if (!(*cmd)->files_to_create)
				return (1); // diffrent error check
		}
		while ((*cmd)->files_to_create[j])	
			j++;
		(*cmd)->files_to_create = ft_realloc((*cmd)->files_to_create, (j + 2));
		if (!(*cmd)->files_to_create)
			return (1);
		(*cmd)->files_to_create[j] = ft_strdup((*cmd)->output_file);
		free((*cmd)->output_file);
	}
	(*cmd)->output_file = ft_strdup(args[i + 1]);
	(*cmd)->append_outfile = 1;
	(*cmd)->args = remove_n_token((*cmd)->args, i, 2);
	return (0);
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
		free(arr);
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
	free(arr);
	return (new_arr);
}
