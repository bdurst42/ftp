#include "ftp.h"

char            *ftp_get_package(int sock, t_header *header)
{
	char        *buff;
	ssize_t     ret;

	if ((ret = recv(sock, header, sizeof(t_header), 0)) > 0)
	{
		if (!(buff = (char*)malloc(sizeof(char) * (MAX_PACKAGE_SIZE + 1))))
			ftp_error(NULL, "malloc failure\n");
		if ((ret = recv(sock, buff, header->nb_bytes, 0)) > 0)
		{
			buff[ret] = '\0';
			return (buff);
		}
	}
	else if (ret == -1)
		ftp_error(NULL, "recv failure\n");
	return (NULL);
}

void		ftp_send_package(char *str, int sock, char flag)
{
	t_header	header;

	header.nb_bytes = ft_strlen(str);
	header.flag = flag;
	if ((send(sock, &header, sizeof(t_header), 0)) == -1)
		ftp_error(NULL, "send failure\n");
	if ((send(sock, str, header.nb_bytes, 0)) == -1)
		ftp_error(NULL, "send failure\n");
}

void		ftp_error(char *format_string, char *str)
{
	if (format_string)
		printf(format_string, str);
	else
		ft_putstr(str);
	exit(-1);
}
