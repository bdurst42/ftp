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
	char	*buff;
	ssize_t	ret;
	int		len;

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
		len = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		(void)len;
		if (!(buff = (char*)malloc(sizeof(char) * (len + 1))))
		{
			ft_putendl("malloc failure");
			exit(-1);
		}
		ft_putnbr(len);
		ft_putendl(" <--------");
		while ((ret = read(fd, buff, len)) > 0)
		{
				  ft_putendl("START");
		ft_putstr("ret = ");
		ft_putnbr(ret);
		ft_putendl("");
			buff[ret] = '\0';
			flag |= F_FILE_NO_END;
			ftp_send_package(buff, sock, flag, ret);
		}
		free(buff);
		ft_putstr("ret = ");
		ft_putnbr(ret);
		ft_putendl("");
		close(fd);
		ft_putendl("HERE");
		ftp_send_package("", sock, 2, -1);
	}
}

static void	ftp_manage_file(char *cmd, char *path, int sock, char flag, char *real_path)
{
	if (cmd)
			ftp_send_package(ft_strjoin("mkfile ",
			path + ft_strlen(real_path)), sock, 2, -1);
	else
		ftp_send_package(ft_strjoin("mkfile ", path + ft_strlen(real_path)), sock, 2, -1);
	ftp_send_file(path, sock, flag);
}

static void	ftp_manage_dir(t_list *list, char *path, int sock, char flag, char *real_path)
{
	t_list	*n;

	if ((flag & F_CLIENT && !(n = ftp_get_sf_in_dir((char*)list->data, -1))) ||
	(!(flag & F_CLIENT) && !(n = ftp_get_sf_in_dir((char*)list->data, sock))))
	{
		ftp_send_package("", sock, 2, -1);
		return ;
	}
	ftp_send_package(ft_strjoin("mkdir ", path + ft_strlen(real_path)), sock, 2, -1);
	ftp_manage_send_cmd(NULL, n, sock, flag, real_path);
}

static void	ftp_manage_sf(char *cmd, t_list **list, int sock, char flag, char *real_path)
{
	char	*path;

	if (ft_strstr((char*)(*list)->data, getcwd(NULL, 0)))
			  path = (char*)(*list)->data + ft_strlen(getcwd(NULL, 0)) + 1;
	else
			  path = (char*)(*list)->data;
		ft_putstr("path = ");
		ft_putendl(path);
	if (ftp_is_dir((char*)(*list)->data))
		ftp_manage_dir(*list, path, sock, flag, real_path);
	else if (ftp_is_file((char*)(*list)->data))
		ftp_manage_file(cmd, path, sock, flag, real_path);
	else
	{
		if (flag & F_CLIENT)
			printf("ERROR: No such file or directory -> %s\n",
			(char*)(*list)->data);
		else	
			ftp_send_package(ft_strjoin("ERROR: No such file or directory -> ",
			(char*)(*list)->data), sock, 2, -1);
	}
}

void		ftp_manage_send_cmd(char *cmd, t_list *list, int sock, char flag, char *real_path)
{
	flag |= F_CONTINUE;
	while (list)
	{
		ft_putstr("str = ");
		ft_putendl((char*)list->data);
		ft_putstr("str without pwd = ");
		ft_putendl((char*)list->data + ft_strlen(getcwd(NULL, 0)));
		if (!real_path)
		{
				  ft_putnbr(ftp_find_last_directory((char*)list->data + ft_strlen(getcwd(NULL, 0))) + 1);
				  ft_putendl("");
			if (ft_strstr((char*)list->data, getcwd(NULL, 0)))
				real_path = ft_strsub((char*)list->data + ft_strlen(getcwd(NULL, 0)) + 1, 0, ftp_find_last_directory((char*)list->data + ft_strlen(getcwd(NULL, 0)) + 1));
			else
				real_path = ft_strsub((char*)list->data, 0, ftp_find_last_directory((char*)list->data));
		}
		ft_putstr("real_path = ");
		ft_putendl(real_path);
		ftp_manage_sf(cmd, &list, sock, flag, real_path);
		list = list->next;
	}
	if (cmd)
		ftp_send_package("", sock, 0, -1);
}
