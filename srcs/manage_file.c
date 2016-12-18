#include "ftp.h"

static t_list	*ftp_get_system_file(char *dir_name, int sock)
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

void				ftp_manage_get_cmd(t_list *list, int sock)
{
	t_list	*tmp;
	char		*cmd;
	t_header	header;

	tmp = list;
	while (tmp)
	{
		header.flag |= F_CONTINUE;
		while (header.flag & F_CONTINUE || header.flag & F_FILE_NO_END)
		{
			cmd = ftp_get_package(sock, &header);
			if (cmd)
			{
				if (!ft_strncmp(cmd, "mkdir ", 6))
					ftp_mkdir(cmd + 6);
				else if (!ft_strncmp(cmd, "mkfile ", 7))
					ftp_get_file(cmd + 7, sock);
			}
		}
		tmp = tmp->next;
	}
}

void				ftp_manage_send_cmd(char *cmd, t_list *list, int sock, char flag)
{
	t_list	*new;
	char		*path;
	char		*current_path;

	current_path = getcwd(NULL, 0);
	while (list)
	{
		ft_putendl("send");
		path = (char*)list->data + ft_strlen(current_path) + 1;
		if (ftp_is_dir((char*)list->data))
		{
			ftp_send_package(ft_strjoin("mkdir ", path), sock, 2, -1);
			if (!(new = ftp_get_system_file((char*)list->data, sock)))
				return ;
			ftp_manage_send_cmd(NULL, new, sock, flag);
		}
		else
		{
		ft_putendl("file");
			ftp_send_package(ft_strjoin("mkfile ", path), sock, 2, -1);
			ftp_send_file(path, sock, flag);
		}
		list = list->next;
		if (cmd)
			ftp_send_package("", sock, 0, -1);
	}
}

void		ftp_send_file(char *file, int sock, char flag)
{
	int		fd;
	char		buff[MAX_PACKAGE_SIZE + 1];
	ssize_t	ret;

	if ((fd = open(file, O_RDONLY)) == -1)
	{
		ftp_send_package("ERROR: open failure", sock, 0, -1);
		ft_putendl("yoloooooo");
		ft_putstr("ERROR: open failure\n");
		ft_putendl("heeeeeeeere");
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
	}
	ftp_send_package("", sock, 0, -1);
}

void            ftp_get_file(char *file, int sock)
{
	t_header    header;
	char        *buff;
	int			fd;

	if (file)
		if ((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0777)) == -1)
		{
			ftp_send_package("ERROR: open failure", sock, 0, -1);
			ft_putendl("wtf");
			ft_putstr("ERROR: open failure\n");
			ft_putendl("mdrmdrmdr");
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
