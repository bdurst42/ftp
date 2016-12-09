#include "ftp.h"

void		ftp_send_file(char *cmd, char *file, int sock, char flag)
{
	int		fd;
	char	buff[MAX_PACKAGE_SIZE + 1];
	ssize_t	ret;

	if ((fd = open(file, O_RDONLY)) == -1)
		ftp_error(NULL, "open failure\n");
	else
	{
		ftp_send_package(cmd, sock, 0);
		while ((ret = read(fd, buff, MAX_PACKAGE_SIZE)) > 0)
		{
			buff[ret] = '\0';
			ft_putendl("send ============================================================");
			ft_putendl(buff);
			flag |= F_CONTINUE;
			ftp_send_package(buff, sock, flag);
		}
		if (ret == -1)
			ftp_error(NULL, "read failure\n");
		else
			ftp_send_package("", sock, 0);
		close(fd);
	}
}

void            ftp_get_file(char *file, int sock)
{
	t_header    header;
	char        *buff;
	int			fd;

	if (file)
		if ((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0777)) == -1)
			ftp_error(NULL, "open failure\n");
	header.flag |= F_CONTINUE;
	while (header.flag & F_CONTINUE)
	{
		buff = ftp_get_package(sock, &header);
		if (buff)
		{
			/*ft_putstr("size = ");
			ft_putnbr(header.nb_bytes);
			ft_putstr("\n");
			*/
			if (header.flag & F_CREATE_FILE)
			{
				ft_putstr(" strlen = ");
				ft_putnbr(ft_strlen(buff));
				ft_putendl("");
				ft_putendl("get ============================================================");
				ft_putstr(buff);
				if ((write(fd, buff, header.nb_bytes)) == -1)
					ftp_error(NULL, "write failure\n");
			}
			else
				ft_putstr(buff);
		}
	}
	if (file)
		close(fd);
}
