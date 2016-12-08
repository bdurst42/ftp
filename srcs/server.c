#include "ftp.h"

static int	ftp_create_server(int port)
{
	int					sock;
	struct protoent		*proto;
	struct sockaddr_in	sin;

	if (!(proto = getprotobyname("tcp")))
		ftp_error(NULL, "getprotobyname failure !\n");
	sock = socket(PF_INET, SOCK_STREAM, proto->p_proto);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if ((bind(sock, (const struct sockaddr*)&sin, sizeof(struct sockaddr_in))) == -1)
		ftp_error(NULL, "bind failure !\n");
	if ((listen(sock, 512)) == -1)
		ftp_error(NULL, "listen failure !\n");
	return (sock);	
}

int				ftp_find_last_directory(char *path)
{
	int	i;

	i = ft_strlen(path);
	while (i > 0 && path[i] != '/')
		--i;
	return (i);
}

char			*ftp_check_path(char *old_path, char *path)
{
	int			i;
	int			j;
	int			pos;
	char		*current_path;

//	ft_putstr("getcwd = ");
//	ft_putendl(getcwd(NULL, 0));
//	ft_putstr("old_path = ");
//	ft_putendl(old_path);
//	ft_putstr("ft_strstr = ");
//	ft_putendl(ft_strstr(getcwd(NULL, 0), old_path));
	current_path = ft_strstr(getcwd(NULL, 0), old_path) + ft_strlen(old_path);
//	ft_putstr("current_path = ");
//	ft_putendl(current_path);
//	if (!ft_strcmp(current_path, ""))
//		current_path = "/";
//	ft_putstr("path = ");
//	ft_putendl(path);
	if (path && path[0] != '/')
		path = ft_strjoin("/", path);
//	ft_putstr("path = ");
//	ft_putendl(path);
	i = -1;
	while (path[++i])
	{
		if (path[i] == '/')
			continue ;
		j = i;
//		ft_putstr("i = ");
//		ft_putnbr(i);
//		ft_putstr(" || j = ");
//		ft_putnbr(j);
//		ft_putstr("\n");
		if (path[j] && path[j++] != '.')
		{
			while (path[j] && path[j] != '/')
				++j;
//			ft_putstr("directory i = ");
//			ft_putnbr(i);
//			ft_putstr(" || j = ");
//			ft_putnbr(j);
//			ft_putstr("\n");
//			ft_putendl(ft_strsub(path, i, j - i));
			current_path = ft_strjoin(current_path, ft_strsub(path, i - 1, j - i + 1));
		}
		else
		{
			while (path[j] == '.')
				if (j++ != i)
					if ((pos = ftp_find_last_directory(current_path)))
						current_path = ft_strsub(current_path, 0, pos);
		}
		i = j;
	}
	current_path = ft_strjoin(old_path, current_path);
//	ft_putendl(current_path);
	return (current_path);
}

void			ftp_cd(char *path, int c_sock)
{
	if ((chdir(path)) == -1)
		ftp_send_package("cd failure", c_sock, 0);
}

char            ftp_is_cmd(char *cmd, int c_sock, char *path)
{
	char	**args;

	path = getcwd(NULL, 0);
	args = ft_strsplit(cmd, ' ');
	ft_putendl(cmd);
	if (!ft_strcmp(cmd, "pwd"))
		ftp_send_package(getcwd(NULL, 0), c_sock, 0);
	else if (!ft_strncmp(cmd, "ls", 2))
	{
		//dup2(c_sock, 1);
		if (execv("/bin/ls", args) == -1)
			ftp_error("NULL", "execve failure\n");
	}
	else if (!ft_strncmp(cmd, "cd ", 3))
	{
		ftp_cd(ftp_check_path(path, ft_strtrim(cmd + 3)), c_sock);
		ftp_send_package("SUCCES: cd", c_sock, 0);
	}
	else if (!ft_strncmp(cmd, "get ", 4))
	{
		ftp_send_file(cmd, c_sock);
		ftp_send_package("SUCCES: get", c_sock, 0);
	}
	else if (!ft_strncmp(cmd, "put ", 4))
	{
		ftp_get_file(ft_strtrim(cmd + 4), c_sock);
		ftp_send_package("SUCCES: put", c_sock, 0);
	}
	else if (!ft_strcmp(cmd, "quit"))
	{
		ftp_send_package("quit", c_sock, 0);
		close(c_sock);
		return (0);
	}
	else
		ftp_send_package("Unknow command !", c_sock, 0);
	return (1);
}

void		ftp_fork(int c_sock)
{
	pid_t		pid;
	t_header	header;
	char		*path;

	path = getcwd(NULL, 0);
	if ((pid = fork()) == -1)
		ftp_send_package("ERROR: fork failure !", c_sock, 0);
	else if (pid)
	{
		ftp_send_package("SUCCES: connection", c_sock, 0);
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
		ftp_error("Usage: s% <port>\n", av[0]);
	if (!(port = ft_atoi(av[1])) && av[1][0] != '0')
		ftp_error(NULL, "Invalid port !\n");
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
