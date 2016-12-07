#ifndef FTP_H
# define FTP_H

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include "../libft/libft.h"

typedef struct	s_header
{
	long		nb_bytes;
}				t_header;

void		ftp_error(char *format_string, char *str);
void		ftp_send_package(char *str, int sock);
char		*ftp_get_package(int sock);

#endif
