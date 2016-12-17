#include "ftp.h"

/*int             ftp_find_last_directory(char *path)
{
	int i;

	i = ft_strlen(path) - 1;
	while (i > 0 && path[i] != '/')
		--i;
	return (i);
}

char				*ftp_get_file_name(char *file_path)
{
	int	len;

	len = ft_strlen(file_path) - 1;
	while (len > -1 && file_path[len] != '/')
		--len;
	return (file_path + len + 1);
}*/

int					ftp_is_dir(char *dir)
{
	struct stat statbuf;

	if (stat(dir, &statbuf) != 0)
		return (0);
	return S_ISDIR(statbuf.st_mode);
}

DIR					*ftp_opendir(char *dir_name, int c_sock)
{
	DIR				*dir;

	if (!(dir = opendir(dir_name)))
	{
		ftp_send_package("ERROR: opendir failure", c_sock, 0, -1);
		return (NULL);
	}
	return (dir);
}
