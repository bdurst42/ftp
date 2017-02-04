/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ftp.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 19:00:00 by bdurst            #+#    #+#             */
/*   Updated: 2017/02/02 15:27:00 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FTP_H
# define FTP_H

# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <dirent.h>
# include <stdio.h>
# include "../libft/libft.h"

# define F_CREATE_FILE	(1 << 0)
# define F_CONTINUE (1 << 1)
# define F_CLIENT	(1 << 2)
# define F_FILE_NO_END	(1 << 3)

# define JOIN ft_strjoin(ft_strjoin(dir_path, "/"), ent->d_name)

# define FILE_BUFFER "tmp"
# define MAX_PACKAGE_SIZE 65000

typedef struct	s_header
{
	long		nb_bytes;
	char		flag;
}				t_header;

typedef struct	s_arg
{
	char		*str;
	char		base;
}				t_arg;

typedef struct	s_last
{
	char		*base_arg;
	int			list_size;
	char		is_opt;
}				t_last;

char			ftp_is_cmd(char *cmd, int c_sock, char *path);

void			ftp_error(char *format_string, char *str, int sock);
void			ftp_send_package(char *str, int sock, char flag, long size);
char			*ftp_get_package(int sock, t_header *header);

void			ftp_get_file(char *file, int sock, char client);
t_list			*ftp_get_sf_in_dir(char *dir_name, int sock);

void			ftp_send_file(char *file, int sock, char flag);

t_list			*ftp_get_args(t_list *args, char opt, char *path);
int				ftp_find_last_directory(char *path);

char			*ftp_check_path(char *old_path, char *path);
char			**ftp_list_to_tabstr(t_list *list);
t_list			*ftp_tabstr_to_list(char **arg);

void			ftp_wildcards(t_list **args, t_list **list, char *path);

int				nmatch(char *s1, char *s2);

DIR				*ftp_opendir(char *dir_name, int c_sock);
int				ftp_is_dir(char *dir);
int				ftp_is_file(char *path);
char			ftp_mkdir(char *dir_name);
char			ftp_rmdir(char *dir_name);

void			ftp_manage_get_cmd(t_list *list, int sock, char client);
void			ftp_manage_send_cmd(char *cmd, t_list *list,
				int sock, char flag);

#endif
