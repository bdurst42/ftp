#ifndef FTP_H
# define FTP_H

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "../libft/libft.h"

typedef struct	s_header
{
	long		nb_bytes;
}				t_header;

void		ftp_error(char *format_string, char *str);
void		ftp_print_sock(char *str, int fd);

#endif
