#include "ftp.h"

char		error(char *s1, char *s2, char *s3)
{
	ft_putstr(s1);
	if (s2)
		ft_putstr(s2);
	if (s3)
		ft_putstr(s3);
	return (-1);
}

static int	create_server(int port)
{
	int					sock;
	struct protoent		*proto;
	struct sockaddr_in	sin;

	if (!(proto = getprotobyname("tcp")))
		return (error("getprotobyname error !", "", ""));
	sock = socket(PF_INET, SOCK_STREAM, proto->p_proto);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if ((bind(sock, (const struct sockaddr*)&sin, sizeof(struct sockaddr_in))) == -1)
		return (error("bind error !", "", ""));
	if ((listen(sock, 42)) == -1)
		return (error("listen error !", "", ""));
	return (sock);	
}

int			main(int ac, char *av[])
{
	int					port;
	int					sock;
	int					c_sock;
	uint32_t			c_sock_len;
	struct sockaddr_in	c_sock_in;

	if (ac != 2)
		return (error("Usage: ", av[0], " <port>\n"));
	if (!(port = ft_atoi(av[1])) && av[1][0] != '0')
		return (error("Invalid port !", "", ""));
	if ((sock = create_server(port)) == -1)
		return (-1);
	if ((c_sock = accept(sock, (struct sockaddr*)&c_sock_in, &c_sock_len)) == -1)
		return (error("accept error !", "", ""));
	close(c_sock);	
	close(sock);
	return (0);
}
