#ifndef FTP_H
# define FTP_H

# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <stdio.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <dirent.h>
# include "../libft/libft.h"

# define F_CREATE_FILE	(1 << 0)
# define F_CONTINUE (1 << 1)
# define F_CLIENT	(1 << 2)

# define FILE_BUFFER "tmp"
# define MAX_PACKAGE_SIZE 65000

typedef struct	s_header
{
	long		nb_bytes;
	char		flag;
}				t_header;

void			ftp_error(char *format_string, char *str, int sock);
void			ftp_send_package(char *str, int sock, char flag, long size);
char			*ftp_get_package(int sock, t_header *header);
void			ftp_get_file(char *file, int sock);
void			ftp_send_files(char *cmd, char **file, int sock, char flag);
t_list			*ftp_get_args(char **args, char opt, char *path, int sock);
char			*ftp_check_path(char *old_path, char *path);
int             ftp_find_last_directory(char *path);
int				nmatch(char *s1, char *s2);
char            **ftp_list_to_tabstr(t_list *list);

#endif
