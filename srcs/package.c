#include "ftp.h"

char	*ftp_get_package(int sock, t_header *header)
{
	char        *buff;
	ssize_t     ret;

	ft_putendl("get pack");
	if ((ret = recv(sock, header, sizeof(t_header), 0)) > 0)
	{
		if (!(buff = (char*)malloc(sizeof(char) * (MAX_PACKAGE_SIZE + 1))))
			ftp_error(NULL, "ERROR: malloc failure\n", 0);
		if ((ret = recv(sock, buff, header->nb_bytes, 0)) > 0)
		{
/*			ft_putstr("get pack = ");
			ft_putendl(buff);
			ft_putstr("size = ");
			ft_putnbr(header->nb_bytes);
			ft_putstr("\n");
*/			buff[ret] = '\0';
			return (buff);
		}
	}
	else if (ret == -1)
		ftp_error(NULL, "ERROR: recv failure\n", 0);
	return (NULL);
}

void	ftp_send_package(char *str, int sock, char flag, long size)
{
	t_header	header;

	ft_putendl("send pack");
	if (size == -1)
		header.nb_bytes = ft_strlen(str);
	else
		header.nb_bytes = size;
	header.flag = flag;
/*	ft_putstr("send pack = ");
	ft_putendl(str);
	ft_putstr("size = ");
	ft_putnbr(header.nb_bytes);
	ft_putstr("\n");
*/	if ((send(sock, &header, sizeof(t_header), 0)) == -1)
		ftp_error(NULL, "ERROR: send failure\n", 0);
	if ((send(sock, str, header.nb_bytes, 0)) == -1)
		ftp_error(NULL, "ERROR: send failure\n", 0);
}

void	ftp_error(char *format_string, char *str, int sock)
{
	if (format_string)
		printf(format_string, str);
	else
		ft_putstr(str);
	if (sock)
		close(sock);
	exit(-1);
}
