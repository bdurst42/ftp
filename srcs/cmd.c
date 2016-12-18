#include "ftp.h"

static void	ftp_cd(char *path, int c_sock)
{
	if ((chdir(path)) == -1)
		ftp_send_package("ERROR: cd failure", c_sock, 0, -1);
	else
		ftp_send_package("SUCCES: cd", c_sock, 0, -1);
}

static void	ftp_ls(char **args, int c_sock)
{
	pid_t	father;
	int		fd;
	int		stat_loc;

	for (int i = 0; args[i]; i++)
	{
		ft_putstr("arg = ");
		ft_putendl(args[i]);
	}
	if ((fd = open(FILE_BUFFER, O_RDWR | O_CREAT | O_TRUNC, 0777)) == -1)	
	{
		ftp_send_package("ERROR: create tmp file fail !", c_sock, 0, -1);
		return ;
	}
	if ((father = fork()) == 0)
	{
		if (dup2(fd, 1) < 0 || dup2(fd, 2) < 0)
			ftp_send_package("ERROR: dup2 failure", c_sock, 0, -1);
		else if (execv("/bin/ls", args) == -1)
		{
			ftp_send_package("ERROR: execv failure", c_sock, 0, -1);
			return ;
		}
	}
	else
	{
		wait4(0, &stat_loc, 0, 0);
		close(fd);
		ftp_send_package("ls", c_sock, 0, -1);
		ftp_send_file(FILE_BUFFER, c_sock, 0);
	}
}

static void	ftp_manage_dir(char *cmd, int c_sock, char del, char (*f)(char *dir_name))
{
	int	i;
	char	**args;
	char	*msg;

	args = ft_strsplit(cmd, ' ');
	msg = NULL;
	i = 0;
	while (args[++i])
		if (f(args[i]) == -1)
			msg = ft_strjoin(ft_strjoin(msg, args[i]), " ");
	if (!msg)
	{
		if (del)
			msg = "SUCCES: rmdir";
		else
			msg = "SUCCES: mkdir";
	}
	else
		if (del)
			msg = ft_strjoin("ERROR: Can't delete ", msg);
		else
			msg = ft_strjoin("ERROR: Can't create ", msg);
	ftp_send_package(msg, c_sock, 0, -1);
}

static char	ftp_quit(int c_sock)
{
	ftp_send_package("quit", c_sock, 0, -1);
	close(c_sock);
	return (0);
}

char            ftp_is_cmd(char *cmd, int c_sock, char *path)
{
	t_list		*list;

	if (!ft_strcmp(cmd, "pwd"))
		ftp_send_package(getcwd(NULL, 0), c_sock, 0, -1);
	else if (!ft_strncmp(cmd, "ls", 2))
		ftp_ls(ftp_list_to_tabstr(ftp_get_args(ft_strsplit(cmd, ' '),
		1, path, c_sock)), c_sock);
	else if (!ft_strncmp(cmd, "cd ", 3))
		ftp_cd(ftp_check_path(path, ft_strtrim(cmd + 3)), c_sock);
	else if (!ft_strncmp(cmd, "get ", 4))
	{
		if ((list = ftp_get_args(ft_strsplit(cmd, ' '), 0, path, c_sock)))
			ftp_manage_send_cmd(cmd, list->next, c_sock, 1);
	}
	else if (!ft_strncmp(cmd, "put ", 4))
	{
		if ((list = ftp_get_args(ft_strsplit(cmd, ' '), 0, NULL, c_sock)))
			ftp_manage_get_cmd(list->next, c_sock);
	}
	else if (!ft_strncmp(cmd, "mkdir ", 6))
		ftp_manage_dir(cmd, c_sock, 0, &ftp_mkdir);
	else if (!ft_strncmp(cmd, "rmdir ", 6))
		ftp_manage_dir(cmd, c_sock, 1, &ftp_rmdir);
	else if (!ft_strcmp(cmd, "quit"))
		return (ftp_quit(c_sock));
	else
		ftp_send_package("Unknow command !", c_sock, 0, -1);
	return (1);
}
