/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maahoff <maahoff@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 17:22:34 by maahoff           #+#    #+#             */
/*   Updated: 2024/11/30 20:13:48 by maahoff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
// error messege !!!
void	ft_error(t_cmd *cmd, char *error_message)
{
	free_all(cmd);
	if (error_message)
		ft_putstr_fd(error_message, 2);
	else
		write(1, "some error apparently\n", 22);
	exit(EXIT_FAILURE);
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