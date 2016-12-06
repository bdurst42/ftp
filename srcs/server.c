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
	if ((listen(sock, 42)) == -1)
		ftp_error(NULL, "listen failure !\n");
	return (sock);	
}

void		ftp_fork(int c_sock)
{
	pid_t	pid;

	if ((pid = fork()) == -1)
		ftp_send("ERROR: fork failure !\n", c_sock);
	else if (pid)
	{
		ftp_send("SUCCES\n", c_sock);
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
