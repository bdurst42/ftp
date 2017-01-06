#include "ftp.h"

static t_list	*ftp_get_sf_in_dir(char *dir_name, int sock)
{
	t_list			*file;
	DIR				*dir;
	struct dirent	*ent;

	file = NULL;
	if ((dir = ftp_opendir(dir_name, sock)))
	{
		while ((ent = readdir(dir)) != NULL)
			if (ent->d_name[0] != '.')
				ft_node_push_back(&file, ft_strjoin(ft_strjoin(dir_name, "/"), ent->d_name));
		closedir(dir);
	}
	return (file);
}

void		ftp_send_file(char *file, int sock, char flag)
{
	int		fd;
	char		buff[MAX_PACKAGE_SIZE + 1];
	ssize_t	ret;

	if ((fd = open(file, O_RDONLY)) == -1)
	{
		if (flag & F_CLIENT)
			printf("ERROR: open failure -> %s\n", file);
		else
			ftp_send_package(ft_strjoin("ERROR: open failure -> ", file), sock, 2, -1);
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

void            ftp_get_file(char *file, int sock, char client)
{
	t_header    header;
	char        *buff;
	int			fd;

	if (file)
		if ((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0777)) == -1)
		{
			if (client)
				printf("ERROR: open failure -> %s\n", file);
			else
				ftp_send_package(ft_strjoin("ERROR: open failure -> ", file), sock, 0, -1);
			return ;
		}
	header.flag |= F_FILE_NO_END;
	while (header.flag & F_FILE_NO_END)
	{
		buff = ftp_get_package(sock, &header);
		if (buff)
		{
			if (header.flag & F_CREATE_FILE)
				write(fd, buff, header.nb_bytes);
			else
				ft_putstr(buff);
		}
	}
	if (file)
		close(fd);
}

void				ftp_manage_get_cmd(t_list *list, int sock, char client)
{
	char		*cmd;
	t_header	header;

	if (list)
	{
		header.flag |= F_CONTINUE;
		while (header.flag & F_CONTINUE)
		{
			cmd = ftp_get_package(sock, &header);
			if (cmd)
			{
				if (!ft_strncmp(cmd, "mkdir ", 6))
				{
						if (ftp_mkdir(cmd + 6) == -1)
						{
							if (client)
								ft_putstr(ft_strjoin("ERROR: Can't create ", cmd + 6));
							else
								ftp_send_package(ft_strjoin("ERROR: Can't create ", cmd + 6), sock, 2, -1);
						}
				}
				else if (!ft_strncmp(cmd, "mkfile ", 7))
					ftp_get_file(cmd + 7, sock, client);
			}
		}
	}
}

void				ftp_manage_send_cmd(char *cmd, t_list *list, int sock, char flag)
{
	t_list	*new;
	char		*path;
	char		*current_path;
	char		*msg;

	current_path = getcwd(NULL, 0);
	flag |= F_CONTINUE;
	while (list)
	{
		msg = "";
		path = (char*)list->data + ft_strlen(current_path) + 1;
		if (ftp_is_dir((char*)list->data))
		{
			if ((flag & F_CLIENT && !(new = ftp_get_sf_in_dir((char*)list->data, -1)))
			|| (!(flag & F_CLIENT) && !(new = ftp_get_sf_in_dir((char*)list->data, sock))))
			{
					ftp_send_package("", sock, 2, -1);
					list = list->next;
					continue ;
			}
			ftp_send_package(ft_strjoin("mkdir ", path), sock, 2, -1);
			ftp_manage_send_cmd(NULL, new, sock, flag);
		}
		else if (ftp_is_file((char*)list->data))
		{
			if (cmd)
			{
				if (!ft_strchr(path, '/'))
					ftp_send_package(ft_strjoin("mkfile ", path + ftp_find_last_directory(path)), sock, 2, -1);
				else
					ftp_send_package(ft_strjoin("mkfile ", path + ftp_find_last_directory(path) + 1), sock, 2, -1);
			}
			else
				ftp_send_package(ft_strjoin("mkfile ", path), sock, 2, -1);
			ftp_send_file(path, sock, flag);
		}
		else
		{
			if (flag & F_CLIENT)
				printf("ERROR: No such file or directory -> %s\n", (char*)list->data);
			else
				msg = ft_strjoin("ERROR: No such file or directory -> ", (char*)list->data);
			ftp_send_package(msg, sock, 2, -1);
		}
		list = list->next;
	}
	if (cmd)
		ftp_send_package("", sock, 0, -1);
}
