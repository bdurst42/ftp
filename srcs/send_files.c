/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_files.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 18:10:44 by bdurst            #+#    #+#             */
/*   Updated: 2017/01/06 18:59:24 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

void		ftp_send_file(char *file, int sock, char flag)
{
	int		fd;
	char	buff[MAX_PACKAGE_SIZE + 1];
	ssize_t	ret;

	if ((fd = open(file, O_RDONLY)) == -1)
	{
		if (flag & F_CLIENT)
			printf("ERROR: open failure -> %s\n", file);
		else
			ftp_send_package(ft_strjoin("ERROR: open failure -> ", file),
			sock, 2, -1);
		return ;
	}
	else
	{
		while ((ret = read(fd, buff, MAX_PACKAGE_SIZE)) > 0)
		{
			buff[ret] = '\0';
			flag |= F_FILE_NO_END;
			ftp_send_package(buff, sock, flag, lseek(fd, 0, SEEK_END));
		}
		close(fd);
		ftp_send_package("", sock, 2, -1);
	}
}

static void	ftp_manage_file(char *cmd, char *path, int sock, char flag)
{
	if (cmd)
	{
		if (!ft_strchr(path, '/'))
			ftp_send_package(ft_strjoin("mkfile ",
			path + ftp_find_last_directory(path)), sock, 2, -1);
		else
			ftp_send_package(ft_strjoin("mkfile ",
			path + ftp_find_last_directory(path) + 1), sock, 2, -1);
	}
	else
		ftp_send_package(ft_strjoin("mkfile ", path), sock, 2, -1);
	ftp_send_file(path, sock, flag);
}

static void	ftp_manage_dir(t_list *list, char *path, int sock, char flag)
{
	t_list	*n;

	if ((flag & F_CLIENT && !(n = ftp_get_sf_in_dir((char*)list->data, -1))) ||
	(!(flag & F_CLIENT) && !(n = ftp_get_sf_in_dir((char*)list->data, sock))))
	{
		ftp_send_package("", sock, 2, -1);
		return ;
	}
	ftp_send_package(ft_strjoin("mkdir ", path), sock, 2, -1);
	ftp_manage_send_cmd(NULL, n, sock, flag);
}

static void	ftp_manage_sf(char *cmd, t_list **list, int sock, char flag)
{
	char	*msg;
	char	*path;

	msg = "";
	path = (char*)(*list)->data + ft_strlen(getcwd(NULL, 0)) + 1;
	if (ftp_is_dir((char*)(*list)->data))
		ftp_manage_dir(*list, path, sock, flag);
	else if (ftp_is_file((char*)(*list)->data))
		ftp_manage_file(cmd, path, sock, flag);
	else
	{
		if (flag & F_CLIENT)
			printf("ERROR: No such file or directory -> %s\n",
			(char*)(*list)->data);
		else
			msg = ft_strjoin("ERROR: No such file or directory -> ",
			(char*)(*list)->data);
		ftp_send_package(msg, sock, 2, -1);
	}
}

void		ftp_manage_send_cmd(char *cmd, t_list *list, int sock, char flag)
{
	flag |= F_CONTINUE;
	while (list)
	{
		ftp_manage_sf(cmd, &list, sock, flag);
		list = list->next;
	}
	if (cmd)
		ftp_send_package("", sock, 0, -1);
}
