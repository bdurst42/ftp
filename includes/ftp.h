#ifndef FTP_H
# define FTP_H

# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <stdio.h>
# include <sys/socket.h>
# include <fcntl.h>
# include "../libft/libft.h"

# define F_CREATE_FILE	(1 << 0)
# define F_CONTINUE (1 << 1)
# define FILE_BUFFER "tmp"

# define MAX_PACKAGE_SIZE 65000

typedef struct	s_header
{
	long		nb_bytes;
	char		flag;
}				t_header;

void			ftp_error(char *format_string, char *str);
void			ftp_send_package(char *str, int sock, char flag);
char			*ftp_get_package(int sock, t_header *header);
void			ftp_get_file(char *cmd, int sock);
void			ftp_send_file(char *cmd, char *file, int sock, char flag);

#endif
