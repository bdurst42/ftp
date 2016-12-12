#include "ftp.h"

void		ftp_send_files(char *cmd, char **file, int sock, char flag)
{
	int			fd;
	uint32_t	i;
	char		buff[MAX_PACKAGE_SIZE + 1];
	ssize_t		ret;

	i = 0;
	ftp_send_package(cmd, sock, 0);
	while (file[i])
	{
//		ft_putstr("send file: file name = ");
//		ft_putendl(file[i]);
		if ((fd = open(file[i], O_RDONLY)) == -1)
			ftp_error(NULL, "send file open failure\n");
		else
		{
			while ((ret = read(fd, buff, MAX_PACKAGE_SIZE)) > 0)
			{
				buff[ret] = '\0';
			//	ft_putendl("send ============================================================");
			//	ft_putendl(buff);
			//	ft_putendl("============================================================");
				flag |= F_CONTINUE;
				ftp_send_package(buff, sock, flag);
			}
			if (ret == -1)
				ftp_error(NULL, "read failure\n");
			close(fd);
		}
		ftp_send_package("", sock, 0);
		++i;
	}
}

void            ftp_get_file(char *file, int sock)
{
	t_header    header;
	char        *buff;
	int			fd;

	if (file)
		if ((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0777)) == -1)
			ftp_error(NULL, "get file open failure\n");
	header.flag |= F_CONTINUE;
	while (header.flag & F_CONTINUE)
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
					ftp_error(NULL, "write failure\n");
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
