#include "../../includes/minishell.h"

static char **get_paths()
{
	char	*path;
	char	**paths;

	path = getenv("PATH");
	paths = ft_split(path, ':');
	if (!paths)
		return (NULL);
	return (paths);
}

static char *construct_path(char *path, char *cmd)
{
	char *full_path;
	char *temp;

	temp = ft_strjoin(path, "/");
	full_path = ft_strjoin(temp, cmd);
	free(temp);
	return (full_path);
}

char *find_command_in_path(char *cmd)
{
	char	**paths;
	char	*full_path;
	int		i;

	i = 0;
	if (cmd[0] == '/' || cmd[0] == '.')
		return (ft_strdup(cmd));
	paths = get_paths();
	if (!paths)
		return (NULL);
	while (paths[i])
	{
		full_path = construct_path(paths[i], cmd);
		if (access(full_path, F_OK) == 0)
		{
			ft_free_split(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	ft_free_split(paths);
	return (NULL);
}