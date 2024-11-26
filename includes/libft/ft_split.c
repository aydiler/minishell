/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 14:33:33 by adiler            #+#    #+#             */
/*   Updated: 2023/12/11 17:21:52 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	free_memory(char **words, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		free(words[i]);
		i++;
	}
	free(words);
}

static int	wordlen(char const *s, char c)
{
	int	len;

	len = 0;
	while (*s != c && *s != '\0')
	{
		s++;
		len++;
	}
	return (len);
}

static int	copy_words(char **words, char c)
{
	int		i;
	int		wordlength;
	char	*tmp;

	i = 0;
	while (words[i] != NULL)
	{
		tmp = words[i];
		wordlength = wordlen(tmp, c);
		words[i] = malloc(wordlength + 1);
		if (!words[i])
		{
			free_memory(words, i);
			return (0);
		}
		ft_memcpy(words[i], tmp, wordlength);
		words[i][wordlength] = '\0';
		i++;
	}
	return (1);
}

static void	get_words(char **words, char const *s, char c)
{
	int		i;

	i = 0;
	if (*s && *s != c)
	{
		words[i] = (char *)s;
		i++;
		s++;
	}
	while (*s)
	{
		if (*s != c && *(s - 1) == c)
		{
			words[i] = (char *)s;
			i++;
		}
		s++;
	}
	words[i] = NULL;
}

char	**ft_split(char const *s, char c)
{
	char	**words;
	int		wordcount;
	int		i;

	if (s == NULL)
		return (NULL);
	i = 0;
	wordcount = 0;
	while (s[i])
	{
		if (s[i] != c && (s[i + 1] == c || s[i + 1] == '\0'))
			wordcount++;
		i++;
	}
	words = ft_calloc(wordcount + 1, sizeof(char *));
	if (!words)
		return (NULL);
	get_words(words, s, c);
	if (!copy_words(words, c))
		return (NULL);
	return (words);
}
