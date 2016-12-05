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

int			main(int ac, char *av[])
{
	int					port;
	int					sock;
	t_header			header;
	char				*buff;
	ssize_t				ret;

	if (ac != 3)
		ftp_error("Usage: s% <addr> <port>\n", av[0]);
	if (!(port = ft_atoi(av[2])) && av[1][0] != '0')
		ftp_error(NULL, "Invalid port !\n");
	sock = ftp_create_client(av[1], port);
	while (1)
	{
		if ((ret = read(sock, &header, sizeof(header))) > 0)
		{
			ft_putnbr(header.nb_bytes);
			ft_putstr("\n");
			if (!(buff = malloc(sizeof(char) * (header.nb_bytes + 1))))
				ftp_error(NULL, "malloc failure\n");
			while ((ret = read(sock, buff, sizeof(header.nb_bytes))) > 0)
			{
					ft_putnbr(ret);
					ft_putstr("\n");
				buff[ret] = '\0';
					ft_putstr("tes2\n");
				if (ret == -1)
					ftp_error(NULL, "read failure\n");
				else
				{
					ft_putstr("test\n");
					write(1, buff, sizeof(buff));
				}
			}
			free(buff);
		}
		else if (ret == -1)
			ftp_error(NULL, "read failure\n");
	}
	close(sock);
	return (0);
}
