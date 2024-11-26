/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 21:27:28 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/26 20:14:44 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	quote_handling(char *line, int *i, int *in_token, char c)
{
	if (line[*i] == '\"')
	{
		(*i)++;
		while (line[*i] && line[*i] != '|' && line[*i] != c)
			(*i)++;
	}
	(*i)++;
	if (*in_token == 2)
		(*i) = (*i) - 1;
	if (line[*i] && line[(*i) + 1] && line[(*i) + 1] != '|' && (line[(*i) + 1] 
			== ' ' || (line[(*i) + 1] >= 9 && line[(*i) + 1] <= 13)))
		*in_token = 1;
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
			quote_handling(line, &i, &in_token, line[i]);
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

void	free_tokens(char **args, int j)
{
	int	i;

	i = 0;
	while (i < j)
	{
		free(args[i]);
		i++;
	}
	free(args);
}

char	*fill_token(char *line, int *i, int l)
{
	char	*token;
	int		j;

	j = 2;
	if (line[*i] == '\"' || line[*i] == '\'')
	{
		quote_handling(line, i, &j, line[*i]);
		l++;
	}
	else
	{
		while (line[*i] && line[*i] != '|' && (line[*i] != ' ' || (line[*i] >= 9
					&& line[*i] <= 13)))
			(*i)++;
	}
	token = malloc(sizeof(char) * (*i - l + 1));
	if (!token)
		return (0);
	j = 0;
	while (l < *i)
		token[j++] = line[l++];
	token[j] = '\0';
	return (token);
}

char	**tokenizer(char *line)
{
	char	**args;
	int		i;
	int		j;

	if (!line)
		return (NULL);
	args = malloc(sizeof(char *) * (count_tokens(line) + 1));
	if (!args)
		return (NULL);
	i = 0;
	j = 0;
	while (line[i] && line[i] != '|')
	{
		if (line[i] != ' ' && !(line[i] >= 9 && line[i] <= 13))
		{
			args[j] = fill_token(line, &i, i);
			if (!args[j])
				return (free_tokens(args, j), NULL);
			j++;
		}
		i++;
	}
	args[j] = 0;
	return (args);
}
