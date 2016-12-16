#include "ftp.h"

static int	ftp_create_client(char *addr, char *port)
{
	int					sock;
	//struct protoent		*proto;
	//struct hostent		*host;
	struct addrinfo		info;
	struct addrinfo		*res;

	//if (!(proto = getprotobyname("tcp")))
	//	ftp_error(NULL, "ERROR: getprotobyname failure !\n", 0);
	//host = gethostbyname(addr);
	//res = NULL;
	ft_putendl("before");
	ft_memset(&info, 0, sizeof(info));
	ft_putendl("after");
	info.ai_family = PF_INET;
	//hint.ai_flags = AI_NUMERICHOST;
	info.ai_socktype = SOCK_STREAM;
	info.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo(addr, port, &info, &res))
	{
		free(res);
		ftp_error(NULL, "ERROR: getaddrinfo failure !\n", 0);
	}
	sock = socket(info.ai_family, info.ai_socktype, info.ai_protocol);
	//res->ai_addr->s_addr = *(unsigned int *)host->h_addr;
	if ((connect(sock, res->ai_addr, res->ai_addrlen)) == -1)
	{
		free(res);
		ftp_error(NULL, "ERROR: connect failure !\n", 0);
	}
	free(res);
	return (sock);	
}

char		*ftp_get_stdin(int sock)
{
	ssize_t		ret;
	char		*line;

	ft_putstr("\033[0;34mftp\033[0m \033[0;32m->\033[0m ");
	if ((ret = gnl(0, &line) > 0))
		return (ft_strtrim(line));
	else if (ret == -1)
		ftp_error(NULL, "ERROR: gnl failure\n", sock);
	return (NULL);
}

void		ftp_parse_cmd(char *cmd, int sock)
{
	t_list	*list;

	if (!ft_strncmp(cmd, "put ", 4))
	{
		list = ftp_get_args(ft_strsplit(cmd, ' '), 0, NULL, sock);
		ftp_manage_send_cmd(cmd, list->next, sock, 1);
	}
	else
		ftp_send_package(cmd, sock, 0, -1);
}

char		ftp_ret_cmd(char *cmd, int sock)
{
	t_list	*list;

	if (!ft_strcmp(cmd, "quit"))
	{
		close(sock);
		return (0);
	}
	else if (!ft_strcmp(cmd, "ls"))
		ftp_get_file(NULL, sock);
	else if (!ft_strncmp(cmd, "get ", 4))
	{
		ft_putendl("GETTTTTTTTTTTTTTTTTTTTTTTTTT ---------------------------");
		list = ftp_get_args(ft_strsplit(cmd, ' '), 0, NULL, sock);
		ftp_manage_get_cmd(list->next, sock);
	}
	else
		ft_putendl(cmd);
	return (1);
}

int			main(int ac, char *av[])
{
//	int					port;
	int					sock;
	char				*cmd;
	t_header			header;

	if (ac != 3)
		ftp_error("Usage: %s <addr> <port>\n", av[0], 0);
	//if (!(port = ft_atoi(av[2])) && av[1][0] != '0')
	///	ftp_error(NULL, "ERROR: Invalid port !\n", 0);
	sock = ftp_create_client(av[1], av[2]);
	while (1)
	{
		if ((cmd = ftp_get_package(sock, &header)))
			if (!(ftp_ret_cmd(cmd, sock)))
				return (0);
		if ((cmd = ftp_get_stdin(sock)))
			ftp_parse_cmd(cmd, sock);
	}
	close(sock);
	return (0);
}
