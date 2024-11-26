/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiler <adiler@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 21:51:20 by adiler            #+#    #+#             */
/*   Updated: 2023/12/11 17:04:56 by adiler           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*new_list;
	t_list	*new_elem;
	void	*new_content;

	if (f == NULL || del == NULL)
		return (NULL);
	new_list = NULL;
	while (lst)
	{
		new_content = f(lst->content);
		if (new_content)
		{
			new_elem = ft_lstnew(new_content);
			if (new_elem)
			{
				ft_lstadd_back(&new_list, new_elem);
				lst = lst->next;
				continue ;
			}
			del(new_content);
		}
		ft_lstclear(&new_list, del);
		return (NULL);
	}
	return (new_list);
}
