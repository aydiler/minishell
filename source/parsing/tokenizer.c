/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 20:53:22 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/10 20:53:23 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	quote_handling(char *line, int *i, int *j, char c)
{
	(*i)++;
	while (line[*i] && line[*i] != c)
		(*i)++;
	if (*j != -1)
		(*i)++;
	if (*j != -1)
		(*j) += 2;
}

int	count_tokens(char *line)
{
	int	cnt;
	int	i;
	int	in_token;
	int	id;

	cnt = 0;
	id = -1;
	i = -1;
	in_token = 0;
	while (line[++i] && line[i] != '|')
	{
		if (line[i] == '\"' || line[i] == '\'')
			quote_handling(line, &i, &id, line[i]);
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
		ft_memdel((void **)&(args[i]));
		i++;
	}
	ft_memdel((void **)&(args));
}

char	*fill_token(char *line, int *i, int l)
{
	char	*token;
	int		j;

	j = 0;
	while (line[*i] && line[*i] != '|' && (line[*i] != ' ' && !(line[*i] >= 9
				&& line[*i] <= 13)))
	{
		if (line[*i] == '\"' || line[*i] == '\'')
			quote_handling(line, i, &j, line[*i]);
		else
			(*i)++;
	}
	token = malloc(sizeof(char) * (*i - l + 1 - j));
	if (!token)
		return (0);
	j = 0;
	while (l < *i)
	{
		if (line[l] == '\"' || line[l] == '\'')
			token = token_cat(token, &j, quote_2_token(line, &l, line[l]));
		else
			token[j++] = line[l++];
	}
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
			continue ;
		}
		i++;
	}
	args[j] = 0;
	return (args);
}
