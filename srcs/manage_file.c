#include "ftp.h"

char	ftp_mkdir(char *dir_name, int sock)
{
		  ft_putendl(dir_name);
	if ((mkdir(dir_name, 0777)) == -1)
	{
		ftp_send_package("ERROR: mkdir failure", sock, 0, -1);
		return (0);
	}
	return (1);
}

t_list		*ftp_get_system_file(char *dir_name, int sock)
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

void		ftp_manage_get_cmd(t_list *list, int sock)
{
	t_list		*tmp;
	char		*cmd;
	t_header	header;

	tmp = list;
	while (tmp)
	{
		header.flag |= F_CONTINUE;
		while (header.flag & F_CONTINUE || header.flag & F_FILE_NO_END)
		{
			cmd = ftp_get_package(sock, &header);
			ft_putstr("get cmd");
			if (cmd)
			{
				ft_putstr(cmd);
			ft_putstr("\n");
			if (!ft_strncmp(cmd, "mkdir ", 6))
				ftp_mkdir(cmd + 6, sock);
			else if (!ft_strncmp(cmd, "mkfile ", 7))
				ftp_get_file(cmd + 7, sock);
			}
		}
		tmp = tmp->next;
	}
}

void		ftp_manage_send_cmd(char *cmd, t_list *list, int sock, char flag)
{
	t_list	*tmp;
	t_list	*new;
	char		*path;
	char		*current_path;

	current_path = getcwd(NULL, 0);
	if (cmd)
		ftp_send_package(cmd, sock, 0, -1);
	tmp = list;
	while (tmp)
	{
		path = (char*)tmp->data + ft_strlen(current_path) + 1;
		ft_putstr("path = ");
		ft_putendl(path);
		if (ftp_is_dir((char*)tmp->data))
		{
			ft_putstr("dir -> ");
			ft_putendl((char*)tmp->data);
			ftp_send_package(ft_strjoin("mkdir ", path), sock, 2, -1);
			if (!(new = ftp_get_system_file((char*)tmp->data, sock)))
				return ;
			ftp_manage_send_cmd(NULL, new, sock, flag);
		}
		else
		{
			ft_putstr("file -> ");
			ft_putendl((char*)tmp->data);
			ftp_send_package(ft_strjoin("mkfile ", path), sock, 2, -1);
			ftp_send_file(path, sock, flag);
			
		}
		tmp = tmp->next;
		if (cmd)
		{
				  ft_putstr("ENDDDDD\n");
				  ftp_send_package("", sock, 0, -1);
		}
	}
}

void		ftp_send_file(char *file, int sock, char flag)
{
	int			fd;
	char		buff[MAX_PACKAGE_SIZE + 1];
	ssize_t		ret;

	ft_putstr("SEND FILE: file name = ");
			ft_putendl(file);
	if ((fd = open(file, O_RDONLY)) == -1)
	{
		ftp_send_package("ERROR: open failure", sock, 0, -1);
		ft_putstr("ERROR: open failure\n");
		return ;
	}
	else
	{
		while ((ret = read(fd, buff, MAX_PACKAGE_SIZE)) > 0)
		{
			buff[ret] = '\0';
			//	ft_putendl("send ============================================================");
			//	ft_putendl(buff);
			//	ft_putendl("============================================================");
			flag |= F_FILE_NO_END;
			ftp_send_package(buff, sock, flag, lseek(fd, 0, SEEK_END));
		}
		close(fd);
		if (ret == -1)
		{
			ftp_send_package("ERROR: read failure", sock, 0, -1);
			ft_putstr("ERROR: read failure\n");
			return ;
		}
	}
	ftp_send_package("", sock, 0, -1);
}

void            ftp_get_file(char *file, int sock)
{
	t_header    header;
	char        *buff;
	int			fd;

	ft_putstr("GET FILE:");
	ft_putendl(file);
	if (file)
		if ((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0777)) == -1)
		{
			ftp_send_package("ERROR: open failure", sock, 0, -1);
			ft_putstr("ERROR: open failure\n");
			return ;
		}
	header.flag |= F_FILE_NO_END;
	while (header.flag & F_FILE_NO_END)
	{
		/*if (file)
		{
			ft_putstr("get file: file name = ");
			ft_putendl(file);
		}*/
		buff = ftp_get_package(sock, &header);
		if (buff)
		{
			//ft_putstr("SEND FILE: size = ");
			//ft_putnbr(header.nb_bytes);
			//ft_putstr("\n");
			if (header.flag & F_CREATE_FILE)
			{
				//ft_putendl("CREATE FILE");
				/*ft_putstr(" strlen = ");
				ft_putnbr(ft_strlen(buff));
				ft_putendl("");
				ft_putendl("get ////////////////////////////////////////////////////");
				ft_putstr(buff);
				ft_putendl("////////////////////////////////////////////////////");*/
				if ((write(fd, buff, header.nb_bytes)) == -1)
				{
					close(fd);
					ftp_send_package("ERROR: write failure", sock, 0, -1);
					ft_putstr("ERROR: write failure\n");
					return ;
				}
			}
			else
			{
		//		ft_putstr("buff ==== ");
				ft_putstr(buff);
			}
		}
	}
	//ft_putstr("get file: EEEENNNNNDDDDDDDD\n");
	if (file)
		close(fd);
}
