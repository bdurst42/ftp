#include "ftp.h"

static int	ftp_create_client(char *addr, int port)
{
	int					sock;
	struct protoent		*proto;
	struct sockaddr_in	sin;

	if (!(proto = getprotobyname("tcp")))
		ftp_error(NULL, "getprotobyname failure !\n");
	sock = socket(PF_INET, SOCK_STREAM, proto->p_proto);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(addr);
	if ((connect(sock, (const struct sockaddr*)&sin, sizeof(struct sockaddr_in))) == -1)
		ftp_error(NULL, "connect failure !\n");
	return (sock);	
}

char		*ftp_get_stdin(void)
{
	ssize_t		ret;
	char		*line;

	if ((ret = gnl(0, &line) > 0))
		return (ft_strtrim(line));
	else if (ret == -1)
		ftp_error(NULL, "gnl failure\n");
	return (NULL);
}

void		ftp_parse_cmd(char *cmd, int sock, char *path)
{
	char	**args;

	if (!ft_strncmp(cmd, "put ", 4))
	{
		args = ftp_get_args(ft_strsplit(cmd, ' '), 0, path);
		ftp_send_files(cmd, args + 1, sock, 1);
	}
	else
		ftp_send_package(cmd, sock, 0);
}

char		ftp_ret_cmd(char *cmd, int sock)
{
	if (!ft_strcmp(cmd, "quit"))
	{
		close(sock);
		return (0);
	}
	else if (!ft_strcmp(cmd, "ls"))
		ftp_get_file(NULL, sock);
	else if (!ft_strncmp(cmd, "get ", 4))
		ftp_get_file(ft_strtrim(cmd + 4), sock);
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
	char				*path;

	if (ac != 3)
		ftp_error("Usage: %s <addr> <port>\n", av[0]);
	if (!(port = ft_atoi(av[2])) && av[1][0] != '0')
		ftp_error(NULL, "Invalid port !\n");
	sock = ftp_create_client(av[1], port);
	ftp_send_package("pwd", sock, 0);
	path = ftp_get_package(sock, &header);
	while (1)
	{
		if ((cmd = ftp_get_package(sock, &header)))
			if (!(ftp_ret_cmd(cmd, sock)))
				return (0);
		if ((cmd = ftp_get_stdin()))
			ftp_parse_cmd(cmd, sock, path);
	}
	close(sock);
	return (0);
}
