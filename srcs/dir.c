/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dir.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 16:13:15 by bdurst            #+#    #+#             */
/*   Updated: 2017/03/20 12:58:03 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

char	ftp_rmdir(char *dir_name)
{
	struct stat st;

	if (!ft_strcmp(dir_name, getcwd(NULL, 0)))
		return (-1);
	if (stat(dir_name, &st) != -1)
	{
		if (rmdir(dir_name) == -1)
			return (-1);
	}
	else
		return (-1);
	return (1);
}

char	ftp_mkdir(char *dir_path)
{
	struct stat st;
	int			i;
	char		*dir_name;

	i = 1;
	while (dir_path[i])
	{
		while (dir_path[i] && dir_path[i] != '/')
			++i;
		dir_name = ft_strsub(dir_path, 0, i);
		if (stat(dir_name, &st) == -1)
		{
			if (mkdir(dir_name, 0777) == -1)
				return (-1);
		}
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

	if (!(dir = opendir(dir_name)))
	{
		if (c_sock == -1)
			printf("ERROR: opendir failure -> %s\n", dir_name);
		else if (c_sock != -2)
			ftp_send_package(ft_strjoin("ERROR: opendir failure -> ",
			dir_name), c_sock, 2, -1);
		return (NULL);
	}
	return (dir);
}

char	*call_dir_function(char del, char *path, char *arg, char *msg)
{
	if (del)
	{
		if (ftp_rmdir(ftp_check_path(path, arg)) == -1)
			msg = ft_strjoin(ft_strjoin(msg, arg), " ");
	}
	else
	{
		if (ftp_mkdir(ftp_check_path(path, arg)) == -1)
			msg = ft_strjoin(ft_strjoin(msg, arg), " ");
	}
	return (msg);
}
