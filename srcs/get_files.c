/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_files.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 18:10:31 by bdurst            #+#    #+#             */
/*   Updated: 2017/01/06 18:22:47 by bdurst           ###   ########.fr       */
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
			if (ent->d_name[0] != '.')
				ft_node_push_back(&file, ft_strjoin(ft_strjoin(dir_name, "/"),
				ent->d_name));
		closedir(dir);
	}
	return (file);
}

void			ftp_get_file(char *file, int sock, char client)
{
	t_header	h;
	char		*b;
	int			fd;

	if (file)
		if ((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0777)) == -1)
		{
			if (client)
				printf("ERROR: open failure -> %s\n", file);
			else
				ftp_send_package(ft_strjoin("ERROR: open failure -> ", file),
				sock, 0, -1);
			return ;
		}
	h.flag |= F_FILE_NO_END;
	while (((b = ftp_get_package(sock, &h)) || b) && h.flag & F_FILE_NO_END)
		if (b)
		{
			if (h.flag & F_CREATE_FILE)
				write(fd, b, h.nb_bytes);
			else
				ft_putstr(b);
		}
	if (file)
		close(fd);
}

void			ftp_manage_get_cmd(t_list *list, int sock, char client)
{
	char		*cmd;
	t_header	header;

	header.flag |= F_CONTINUE;
	while (list && header.flag & F_CONTINUE)
	{
		cmd = ftp_get_package(sock, &header);
		if (cmd)
		{
			if (!ft_strncmp(cmd, "mkdir ", 6))
			{
				if (ftp_mkdir(cmd + 6) == -1)
				{
					if (client)
						ft_putstr(ft_strjoin("ERROR: Can't create ",
									cmd + 6));
					else
						ftp_send_package(ft_strjoin("ERROR: Can't create ",
									cmd + 6), sock, 2, -1);
				}
			}
			else if (!ft_strncmp(cmd, "mkfile ", 7))
				ftp_get_file(cmd + 7, sock, client);
		}
	}
}
