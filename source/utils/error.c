/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ubuntu <ubuntu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 17:22:34 by maahoff           #+#    #+#             */
/*   Updated: 2025/01/16 22:34:14 by ubuntu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
// error messege !!!
void	ft_error(t_cmd **cmd, int exit_status)
{
	if (cmd && *cmd)
		free_all(cmd);
	if (exit_status == ERR_INVAL)
		ft_putstr_fd("invalid arguments\n", 2);
	else if (exit_status == ERR_NOMEM)
		ft_putstr_fd("Memory allocation error\n", 2);
	else if (exit_status == ERR_PIPE)
		ft_putstr_fd("Pipe or redirection error\n", 2);
	else if (exit_status == ERR_SIGINT)
		return ;
	else if (exit_status == 0)
		return ;
	else
	{
		printf("Error: %d\n", exit_status);
	}
}
/*
errors to handel:

command not found
Befehl existiert nicht oder kann nicht gefunden werden.

invalid arguments
Befehl wurde mit falschen oder unzureichenden Argumenten aufgerufen.

no such file or directory
angegebene Verzeichnis oder die Datei existiert nicht.

permission denied
Benutzer hat nicht die erforderlichen Berechtigungen.

incorrect argument count
Befehl wurde mit der falschen Anzahl von Argumenten aufgerufen.

fork error
Fehler beim Erstellen eines neuen Prozesses.

exec error
Fehler beim Ausführen eines Programms oder Befehls.

signal error
Ein Signal unterbrach das Programm.

memory allocation error
Fehler bei der Speicherzuweisung.

pipe/redirection error
Fehler bei der Umleitung von Ein- oder Ausgaben.

*/