/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_files.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 18:10:31 by bdurst            #+#    #+#             */
/*   Updated: 2017/06/06 01:37:22 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

t_list			*ftp_get_sf_in_dir(char *dir_name, int sock)
{
	t_list			*file;
	DIR				*dir;
	struct dirent	*ent;

	file = NULL;
	if ((dir = ftp_opendir(dir_name, sock)))
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_name[0] != '.')
				ft_node_push_back(&file, ft_strjj(dir_name, "/", ent->d_name));
		}
		closedir(dir);
	}
	else
		file = (void*)-1;
	return (file);
}

static int		ftp_open_file(char *file, int sock, char client)
{
	char		*to_free;
	int			fd;

	if ((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0777)) == -1)
	{
		to_free = ft_strjoin("ERROR: open failure -> ", file);
		if (client)
			printf("%s\n", to_free);
		else
			ftp_send_package(to_free, sock, 0, -1);
		free(to_free);
	}
	return (fd);
}

void			ftp_get_file(char *file, int sock, char client)
{
	t_header	h;
	char		*b;
	int			fd;

	if (file && (fd = ftp_open_file(file, sock, client)) == -1)
		return ;
	h.flag |= F_FILE_NO_END;
	while (h.flag & F_FILE_NO_END && (b = ftp_get_package(sock, &h)))
		if (b)
		{
			if (h.flag & F_CREATE_FILE)
				write(fd, b, h.nb_bytes);
			else
				ft_putstr(b);
			free(b);
		}
	if (file)
		close(fd);
}

static void		ftp_mkdir_fail(char *cmd, int sock, char client)
{
	char		*to_free;

	if (ftp_mkdir(cmd) == -1)
	{
		to_free = ft_strjoin("ERROR: Can't create ", cmd);
		if (client)
			ft_putendl(to_free);
		else
			ftp_send_package(to_free, sock, 2, -1);
		free(to_free);
	}
}

void			ftp_manage_get_cmd(t_list *list, int sock, char client)
{
	char		*cmd;
	t_header	header;

	header.flag |= F_CONTINUE;
	while (list && header.flag & F_CONTINUE)
	{
		if ((cmd = ftp_get_package(sock, &header)))
		{
			if (!ft_strncmp(cmd, "mkdir ", 6))
				ftp_mkdir_fail(cmd + 6, sock, client);
			else if (!ft_strncmp(cmd, "mkfile ", 7))
				ftp_get_file(cmd + 7, sock, client);
			free(cmd);
		}
	}
}
