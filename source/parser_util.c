/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_util.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 17:16:28 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/29 21:57:22 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

int	check_non_args(char *token)
{
	if (!ft_strncmp(">", token, ft_strlen(token)))
		return (1);
	else if (!ft_strncmp("<", token, ft_strlen(token)))
		return (1);
	else if (!ft_strncmp(">>", token, ft_strlen(token)))
		return (1);
	else if (!ft_strncmp(";", token, ft_strlen(token)))
		return (1);
	else if (!ft_strncmp("\\", token, ft_strlen(token)))
		return (1);
	return (0);
}

char	**remove_token(char **args, int i)
{
	if (!args || !args[i])
		return (args);
	free(args[i]);
	while (args[i])
	{
		args[i] = args[i + 1];
		i++;
	}
	args[i] = NULL;
	return (args);
}

char	**remove_n_token(char **args, int i, int n)
{
	int	j;

	if (!args || !args[i] || n <= 0 || !args[i + n - 1])
		return (args);
	while (args[i] && n > 0)
	{
		free(args[i]);
		j = i;
		while (args[j])
		{
			args[j] = args[j + 1];
			j++;
		}
		args[j] = NULL;
		n--;
	}
	return (args);
}
