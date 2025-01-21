/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 13:40:31 by adiler            #+#    #+#             */
/*   Updated: 2025/01/21 19:03:53 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_list_str
{
	char				*line;
	struct s_list_str	*next;
}						t_list_str;
char					*get_next_line(int fd);
void					*ft_memcpy(void *dest, const void *src, size_t n);
char					*ft_strchr(const char *s, int c);
char					*ft_strjoin(char const *s1, char const *s2);
void					ft_lstadd_back_c(t_list_str **lst,
							t_list_str *new_node);
void					handle_read_result(int result, t_list_str **head,
							char **line);
void					load_history(void);
void					save_history(char *line);

#endif