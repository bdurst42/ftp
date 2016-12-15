#include "ftp.h"

static int	ftp_create_server(int port)
{
	int					sock;
	struct protoent		*proto;
	struct sockaddr_in	sin;

	if (!(proto = getprotobyname("tcp")))
		ftp_error(NULL, "ERROR: getprotobyname failure !\n", 0);
	sock = socket(PF_INET, SOCK_STREAM, proto->p_proto);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if ((bind(sock, (const struct sockaddr*)&sin, sizeof(struct sockaddr_in))) == -1)
		ftp_error(NULL, "ERROR: bind failure !\n", 0);
	if ((listen(sock, 512)) == -1)
		ftp_error(NULL, "ERROR: listen failure !\n", 0);
	return (sock);	
}

void			ftp_cd(char *path, int c_sock)
{
	if ((chdir(path)) == -1)
		ftp_send_package("ERROR: cd failure", c_sock, 0, -1);
	else
		ftp_send_package("SUCCES: cd", c_sock, 0, -1);
}

void			ftp_ls(char **args, int c_sock)
{
	pid_t	father;
	int		fd;
	int		stat_loc;

	if ((fd = open(FILE_BUFFER, O_RDWR | O_CREAT | O_TRUNC, 0777)) == -1)	
	{
		ftp_send_package("ERROR: create tmp file fail !", c_sock, 0, -1);
		return ;
	}
	if ((father = fork()) == 0)
	{
		if (dup2(fd, 1) < 0)
		{
			ftp_send_package("ERROR: dup2 failure", c_sock, 0, -1);
			return ;
		}
		if (execv("/bin/ls", args) == -1)
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

char            ftp_is_cmd(char *cmd, int c_sock, char *path)
{
	char		**args;
	t_list		*list;

//	ft_putendl("IS CMMDDDDDDDDDDDDDDDDDDD");
	if (!ft_strcmp(cmd, "pwd"))
		ftp_send_package(getcwd(NULL, 0), c_sock, 0, -1);
	else if (!ft_strncmp(cmd, "ls", 2))
	{
		args = ftp_list_to_tabstr(ftp_get_args(ft_strsplit(cmd, ' '), 1, path, c_sock));
		ftp_ls(args, c_sock);
	}
	else if (!ft_strncmp(cmd, "cd ", 3))
	{
		ftp_cd(ftp_check_path(path, ft_strtrim(cmd + 3)), c_sock);
	}
	else if (!ft_strncmp(cmd, "get ", 4))
	{
		list = ftp_get_args(ft_strsplit(cmd, ' '), 0, path, c_sock);
		ft_putendl("GETTTTTTTTTTTTTTT");
		ft_putendl(cmd);
		ftp_manage_send_cmd(cmd, list->next, c_sock, 1);
		ft_putendl("WTF AFTERR");
	}
	else if (!ft_strncmp(cmd, "put ", 4))
	{
		list = ftp_get_args(ft_strsplit(cmd, ' '), 0, NULL, c_sock);
		ftp_manage_get_cmd(list->next, c_sock);
	}
	else if (!ft_strcmp(cmd, "quit"))
	{
		ftp_send_package("quit", c_sock, 0, -1);
		close(c_sock);
		return (0);
	}
	else
		ftp_send_package("Unknow command !", c_sock, 0, -1);
	return (1);
}

void		ftp_fork(int c_sock)
{
	pid_t		pid;
	t_header	header;
	char		*path;

	path = getcwd(NULL, 0);
	if ((pid = fork()) == -1)
		ftp_send_package("ERROR: fork failure !", c_sock, 0, -1);
	else if (pid != 0)
	{
		ftp_send_package("SUCCES: connection", c_sock, 0, -1);
		while (ftp_is_cmd(ftp_get_package(c_sock, &header), c_sock, path))
			;
	}
}

int			main(int ac, char *av[])
{
	int					port;
	int					sock;
	int					c_sock;
	uint32_t			c_sock_len;
	struct sockaddr_in	c_sock_in;

	if (ac != 2)
		ftp_error("Usage: s% <port>\n", av[0], 0);
	if (!(port = ft_atoi(av[1])) && av[1][0] != '0')
		ftp_error(NULL, "ERROR: Invalid port !\n", 0);
	if ((sock = ftp_create_server(port)) == -1)
		return (-1);
	while (1)
	{
		if ((c_sock = accept(sock, (struct sockaddr*)&c_sock_in, &c_sock_len)) == -1)
			ft_putstr("ERROR: accept failure !\n");
		ftp_fork(c_sock);
	}
	close(c_sock);	
	close(sock);
	return (0);
}
