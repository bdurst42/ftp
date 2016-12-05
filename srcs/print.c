#include "ftp.h"

void		ftp_print_sock(char	*str, int fd)
{
	t_header	header;

	ft_putstr("okay\n");
	header.nb_bytes = ft_strlen(str);
	ft_fputstr((char*)&header, fd);
	ft_fputstr(str, fd);
}

void		ftp_error(char *format_string, char *str)
{
	if (format_string)
		printf(format_string, str);
	else
		ft_putstr(str);
	exit(-1);
}
