#include "ftp.h"

void		ftp_send_file(char *cmd, int sock)
{
	int		fd;
	char	flag;
	char	buff[MAX_PACKAGE_SIZE + 1];
	ssize_t	ret;

	if ((fd = open(ft_strtrim(cmd + 4), O_RDONLY)) == -1)
		ftp_error(NULL, "open failure\n");
	else
	{
		ftp_send_package(cmd, sock, 0);
		while ((ret = read(fd, buff, MAX_PACKAGE_SIZE)) > 0)
		{
			buff[ret] = '\0';
		ft_putendl("send ============================================================");
			ft_putendl(buff);
			flag |= F_GET_FILE;
			flag |= F_CONTINUE;
			ftp_send_package(buff, sock, flag);
		}
		if (ret == -1)
			ftp_error(NULL, "read failure\n");
		else
			ftp_send_package("", sock, 0);
	}
	close(fd);
}

void            ftp_get_file(char *cmd, int sock)
{
	t_header    header;
	char        *buff;
	int			fd;

	ft_putendl(cmd);
	if ((fd = open(cmd, O_WRONLY | O_CREAT | O_TRUNC, 0777)) == -1)
		ftp_error(NULL, "open failure\n");
	header.flag |= F_CONTINUE;
	while (header.flag & F_CONTINUE)
	{
		buff = ftp_get_package(sock, &header);
		ft_putstr("size = ");
		ft_putnbr(header.nb_bytes);
		if (header.flag)
		{
			ft_putstr(" strlen = ");
			ft_putnbr(ft_strlen(buff));
			ft_putendl("");
			ft_putendl("get ============================================================");
			ft_putendl(buff);
			if ((write(fd, buff, header.nb_bytes)) == -1)
				ftp_error(NULL, "write failure\n");
		}
		ft_putendl("");
	}
	close(fd);
}