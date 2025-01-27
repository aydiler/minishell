/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_symtab.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/16 14:56:05 by maweiss           #+#    #+#             */
/*   Updated: 2024/09/25 12:20:48 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ms_symtab.h: Header file responsible for all the structures necessary to work with the symbol table. */


#ifndef MS_SYMTAB_H
# define MS_SYMTAB_H
# include "minishell.h"

typedef struct s_symtab {
	char				*key;
	char				*value;
	struct s_symtab		*next;
}				t_symtab;

typedef struct s_symtab_stack {
	int						level;
	t_symtab				**symtab;
	int						size;
	int						used;
	float					load_factor;
	struct s_symtab_stack	*next;
}				t_symtab_stack;

#endif
