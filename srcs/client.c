#include "ftp.h"

static int	ftp_create_client(char *addr, int port)
{
	int					sock;
	struct protoent		*proto;
	struct sockaddr_in	sin;
	struct hostent		*host;

	if (!(proto = getprotobyname("tcp")))
		ftp_error(NULL, "getprotobyname failure !\n");
	host = gethostbyname(addr);
	sock = socket(PF_INET, SOCK_STREAM, proto->p_proto);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = *(unsigned int *)host->h_addr;
	if ((connect(sock, (const struct sockaddr*)&sin, sizeof(struct sockaddr_in))) == -1)
		ftp_error(NULL, "connect failure !\n");
	return (sock);	
}

char		*ftp_get_stdin(void)
{
	ssize_t		ret;
	char		*line;

	ft_putstr("\033[0;34mftp\033[0m \033[0;32m->\033[0m ");
	if ((ret = gnl(0, &line) > 0))
		return (ft_strtrim(line));
	else if (ret == -1)
		ftp_error(NULL, "gnl failure\n");
	return (NULL);
}

void		ftp_parse_cmd(char *cmd, int sock)
{
	char	**args;

	if (!ft_strncmp(cmd, "put ", 4))
	{
		args = ftp_get_args(ft_strsplit(cmd, ' '), 0, NULL);
		ftp_send_files(cmd, args + 1, sock, 1);
	}
	else
		ftp_send_package(cmd, sock, 0);
}

char		ftp_ret_cmd(char *cmd, int sock)
{
	char	**args;
	int		i;

	if (!ft_strcmp(cmd, "quit"))
	{
		close(sock);
		return (0);
	}
	else if (!ft_strcmp(cmd, "ls"))
		ftp_get_file(NULL, sock);
	else if (!ft_strncmp(cmd, "get ", 4))
	{
		i = 1;
		args = ftp_get_args(ft_strsplit(cmd, ' '), 0, NULL);
		while (args[i])
			ftp_get_file(args[i++], sock);
	}
	else
		ft_putendl(cmd);
	return (1);
}

int			main(int ac, char *av[])
{
	int					port;
	int					sock;
	char				*cmd;
	t_header			header;

	if (ac != 3)
		ftp_error("Usage: %s <addr> <port>\n", av[0]);
	if (!(port = ft_atoi(av[2])) && av[1][0] != '0')
		ftp_error(NULL, "Invalid port !\n");
	sock = ftp_create_client(av[1], port);
	while (1)
	{
		if ((cmd = ftp_get_package(sock, &header)))
			if (!(ftp_ret_cmd(cmd, sock)))
				return (0);
		if ((cmd = ftp_get_stdin()))
			ftp_parse_cmd(cmd, sock);
	}
	close(sock);
	return (0);
}
