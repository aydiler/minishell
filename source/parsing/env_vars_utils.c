/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 17:37:49 by maahoff           #+#    #+#             */
/*   Updated: 2024/12/14 16:03:17 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	is_env_var(char c)
{
	if (c == ' ' || (c >= 9 && c <= 13) || c == '|' || c == '\0' || 
		c == '$' || c == ';' || c == '/' || c == '\"' || c == '\'')
		return (0);
	return (1);
}

int	is_tilde(char *line, int i)
{
	if (line[i] != '~')
		return (0);
	if (line[i + 1] && (line[i + 1] != ' ' && !(line[i + 1] >= 9 && line[i + 1] 
				<= 13) && line[i + 1] != '|' && line[i + 1] != ';' && 
			line[i + 1] != '/'))
		return (0);
	if (i > 0 && (line[i - 1] != ' ' && !(line[i - 1] >= 9 && line[i - 1] 
				<= 13) && line[i - 1] != '|' && line[i - 1] != ';' && 
			line[i - 1] != '/'))
		return (0);
	return (1);
}

int	exchange_tilde(char **line, int i)
{
	size_t	new_len;
	char	*new_str;

	new_len = ft_strlen(*line) - 1 + ft_strlen("$HOME");
	new_str = malloc(sizeof(char) * (new_len + 1));
	if (!new_str)
		return (ERR_NOMEM);
	ft_strncpy(new_str, *line, i);
	ft_strncpy(new_str + i, "$HOME", ft_strlen("$HOME"));
	ft_strncpy(new_str + i + ft_strlen("$HOME"), *line + i + 1, 
		ft_strlen(*line + i + 1));
	new_str[new_len] = '\0';
	ft_memdel((void **)&(*line));
	*line = ft_strdup(new_str);
	ft_memdel((void **)&(new_str));
	return (0);
}

int	skip_quote(char *line, int *i)
{
	char	c;

	c = line[*i];
	if (line[*i])
		(*i)++;
	while (line[*i] && line[*i] != c)
		(*i)++;
	if (!line[*i])
		return (ERR_UNMATCHED);
	return (0);
}

char	*getenv_range(char *start, size_t *len_var)
{
	char	*env_var;
	char	*temp;

	while (is_env_var(start[*len_var]))
		(*len_var)++;
	temp = ft_strn(start, *len_var);
	env_var = getenv(temp);
	ft_memdel((void **)&(temp));
	if (!env_var)
		return (NULL);
	return (env_var);
}
