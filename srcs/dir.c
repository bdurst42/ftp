#include "ftp.h"

char	ftp_rmdir(char *dir_name)
{
	struct stat st;

	if (stat(dir_name, &st) != -1)
	{
		if (rmdir(dir_name) == -1)
			return (-1);
	}
	else
		return (-1);
	return (1);
}

char	ftp_mkdir(char *dir_name)
{
	struct stat st;

	if (stat(dir_name, &st) == -1)
	{
		if (mkdir(dir_name, 0777) == -1)
			return (-1);
	}
	else
		return (-1);
	return (1);
}

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
