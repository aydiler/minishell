/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 21:27:28 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/26 00:00:16 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	quote_handling(char *line, int *i, int in_token)
{
	if (line[*i] == '\"')
	{
		(*i)++;
		while (line[*i] && line[*i] != '|' && line[*i] != '\"')
			(*i)++;
	}
	if (line[*i] == '\'')
	{
		(*i)++;
		while (line[*i] && line[*i] != '|' && line[*i] != '\'')
			(*i)++;
	}
	(*i)++;
	if (line[(*i) + 1] && line[(*i) + 1] != '|' && (line[(*i) + 1] == ' ' 
			|| (line[(*i) + 1] >= 9 && line[(*i) + 1] <= 13)))
		in_token = 1;
	return (in_token);
}

int	count_tokens(char *line)
{
	int	cnt;
	int	i;
	int	in_token;

	cnt = 0;
	i = -1;
	in_token = 0;
	while (line[++i] && line[i] != '|')
	{
		if (line[i] == '\"' || line[i] == '\'')
			in_token = quote_handling(line, &i, in_token);
		if (line[i] == ' ' || (line[i] >= 9 && line[i] <= 13))
		{
			if (in_token == 1)
				cnt++;
			in_token = 0;
		}
		else
			in_token = 1;
	}
	if ((!line[i] || line[i] == '|') && in_token == 1)
		cnt++;
	return (cnt);
}

char	**tokenizer(char *line)
{
	char	**args;

	if (!line)
		return (NULL);
	args = malloc(sizeof(char *) * count_tokens(line));
	if (!args)
		return (NULL);
	return (args);
}
