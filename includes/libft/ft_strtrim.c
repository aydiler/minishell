/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 11:30:11 by adiler            #+#    #+#             */
/*   Updated: 2023/12/08 20:26:21 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	get_start(char const *str, char const *set, int strlen, int setlen);
static int	get_end(char const *str, char const *set, int strlen, int setlen);

char	*ft_strtrim(char const *s1, char const *set)
{
	int		start;
	int		end;
	int		len;
	char	*str;

	start = get_start(s1, set, ft_strlen(s1), ft_strlen(set));
	end = get_end(s1, set, ft_strlen(s1), ft_strlen(set));
	len = end - start + 1;
	if (len <= 0)
		return (ft_strdup(""));
	str = ft_substr(s1, start, len);
	return (str);
}

static int	get_start(char const *str, char const *set, int strlen, int setlen)
{
	int	i;
	int	j;
	int	found;

	i = 0;
	while (i < strlen)
	{
		j = 0;
		found = 1;
		while (j < setlen)
		{
			if (str[i] == set[j])
			{
				found = 0;
				break ;
			}
			j++;
		}
		if (found)
			break ;
		i++;
	}
	return (i);
}

static int	get_end(char const *str, char const *set, int strlen, int setlen)
{
	int	i;
	int	j;
	int	found;

	i = strlen - 1;
	while (i >= 0)
	{
		j = setlen - 1;
		found = 1;
		while (j >= 0)
		{
			if (str[i] == set[j])
			{
				found = 0;
				break ;
			}
			j--;
		}
		if (found)
			break ;
		i--;
	}
	return (i);
}
