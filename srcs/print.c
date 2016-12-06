#include "ftp.h"

void		ftp_send(char	*str, int sock)
{
	t_header	header;

	header.nb_bytes = ft_strlen(str);
	send(sock, &header, sizeof(t_header), 0);
	send(sock, str, header.nb_bytes, 0);
}

void		ftp_error(char *format_string, char *str)
{
	if (format_string)
		printf(format_string, str);
	else
		ft_putstr(str);
	exit(-1);
}
