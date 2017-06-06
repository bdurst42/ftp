/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dir.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 16:13:15 by bdurst            #+#    #+#             */
/*   Updated: 2017/06/06 01:37:44 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

char	ftp_rmdir(char *dir_name)
{
	struct stat st;
	char		*c_p;
	int			ret;

	c_p = getcwd(NULL, 0);
	ret = 1;
	if (ft_strcmp(dir_name, c_p) && stat(dir_name, &st) != -1)
	{
		if (rmdir(dir_name) == -1)
			ret = -1;
	}
	else
		ret = -1;
	free(c_p);
	return (ret);
}

char	ftp_mkdir(char *dir_path)
{
	struct stat st;
	size_t		i;
	char		*dir_name;
	int			ret;

	i = 1;
	while (dir_path[i])
	{
		while (dir_path[i] && dir_path[i] != '/')
			++i;
		dir_name = ft_strsub(dir_path, 0, i);
		if (((ret = stat(dir_name, &st)) != -1 && i == ft_strlen(dir_path)) || (ret == -1 && mkdir(dir_name, 0777) == -1))
			return (-1);
		free(dir_name);
		++i;
	}
	return (1);
}

int		ftp_is_dir(char *dir)
{
	struct stat statbuf;

	if (stat(dir, &statbuf) != 0)
		return (0);
	return (S_ISDIR(statbuf.st_mode));
}

DIR		*ftp_opendir(char *dir_name, int c_sock)
{
	DIR				*dir;
	char			*to_free;

	if (!(dir = opendir(dir_name)))
	{
		to_free = ft_strjoin("ERROR: opendir failure -> ", dir_name);
		if (c_sock == -1)
			printf("%s\n", to_free);
		else if (c_sock != -2)
			ftp_send_package(to_free, c_sock, 2, -1);
		free(to_free);
		return (NULL);
	}
	return (dir);
}

char	*call_dir_function(char del, char *path, char *arg, char *msg)
{
	char	*to_free;

	if (del)
	{
		if (ftp_rmdir(ftp_check_path(path, arg)) == -1)
		{
			to_free = ft_strjoin(msg, arg + 1);
			msg = ft_strjoin(to_free, " ");
			free(to_free);
		}
	}
	else
	{
		if (ftp_mkdir(ftp_check_path(path, arg)) == -1)
		{
			to_free = ft_strjoin(msg, arg + 1);
			msg = ft_strjoin(to_free, " ");
			free(to_free);
		}
	}
	return (msg);
}
