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

int			main(int ac, char *av[])
{
	int					port;
	int					sock;
	char				*buff;

	if (ac != 3)
		ftp_error("Usage: s% <addr> <port>\n", av[0]);
	if (!(port = ft_atoi(av[2])) && av[1][0] != '0')
		ftp_error(NULL, "Invalid port !\n");
	sock = ftp_create_client(av[1], port);
	while (1)
	{
		buff = ftp_get_package(sock);
		if (buff)
		{
			if (!ft_strcmp(buff, "quit"))
			{
				close(sock);
				exit(0);
			}
			else
				ft_putendl(buff);
		}
			if ((buff = ftp_get_stdin()))
			ftp_send_package(buff, sock);
	}
	close(sock);
	return (0);
}
