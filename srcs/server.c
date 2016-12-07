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

void			ftp_get_file(char **args, int c_sock)
{
	;	
}

void			ftp_put_file(char **args, int c_sock)
{
	int fd;

	if ((fd = open(ft_strtrim(args[1]), O_RDONLY)) == -1)
	{
		ftp_send_package("ERROR: We can't open this file", c_sock);
		ftp_error("NULL", "open failure\n");
	}
	else
	{
		
	}
}

char            ftp_is_cmd(char *cmd, int c_sock)
{
	char	**args;

	args = ft_strsplit(cmd, ' ');
	if (!ft_strcmp(cmd, "pwd"))
		ftp_send_package(getcwd(NULL, 0), c_sock);
	else if (!ft_strncmp(cmd, "ls", 2))
	{
		//dup2(c_sock, 1);
		if (execv("/bin/ls", args) == -1)
			ftp_error("NULL", "execve failure\n");
	}
	else if (!ft_strncmp(cmd, "cd", 2))
		ftp_send_package("SUCCES: cd", c_sock);
	else if (!ft_strncmp(cmd, "get ", 4))
	{
		ftp_send_package("SUCCES: get", c_sock);
		ftp_get_file(args, c_sock);
	}
	else if (!ft_strncmp(cmd, "put ", 4))
	{
		ftp_send_package("SUCCES: put", c_sock);
		ftp_put_file(args, c_sock);
	}
	else if (!ft_strcmp(cmd, "quit"))
	{
		ftp_send_package("quit", c_sock);
		close(c_sock);
		return (0);
	}
	else
		ftp_send_package("SUCCES: Unknow command !", c_sock);
	return (1);
}

void		ftp_fork(int c_sock)
{
	pid_t		pid;

	if ((pid = fork()) == -1)
		ftp_send_package("ERROR: fork failure !", c_sock);
	else if (pid)
	{
		ftp_send_package("SUCCES: connection", c_sock);
		while (ftp_is_cmd(ftp_get_package(c_sock), c_sock))
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
