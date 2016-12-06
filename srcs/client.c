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

void			ftp_get_server_package(int sock, t_header *header)
{
	char		*buff;
	ssize_t	ret;
	
	ft_putendl("package");
		ft_putnbr(sock);
		ft_putstr("\n");
	if ((ret = recv(sock, header, sizeof(t_header), 0)) > 0)
	{
		ft_putnbr(ret);
		ft_putstr("\n");
		ft_putnbr(header->nb_bytes);
		ft_putstr("\n");
		if (!(buff = malloc(sizeof(char) * (header->nb_bytes + 1))))
			ftp_error(NULL, "malloc failure\n");
		if ((ret = recv(sock, buff, sizeof(header->nb_bytes), 0)) > 0)
		{
			buff[ret] = '\0';
			if (ret == -1)
				ftp_error(NULL, "read failure\n");
			else
			{
				write(1, buff, sizeof(buff));
			}
		}
		free(buff);
	}
	else if (ret == -1)
		ftp_error(NULL, "read failure\n");
	/*if ((ret = read(sock, header, sizeof(t_header))) > 0)
	{
		ft_putnbr(ret);
		ft_putstr("\n");
		ft_putnbr(header->nb_bytes);
		ft_putstr("\n");
		if (!(buff = malloc(sizeof(char) * (header->nb_bytes + 1))))
			ftp_error(NULL, "malloc failure\n");
		if ((ret = read(sock, buff, sizeof(header->nb_bytes))) > 0)
		{
			buff[ret] = '\0';
			if (ret == -1)
				ftp_error(NULL, "read failure\n");
			else
			{
				write(1, buff, sizeof(buff));
			}
		}
		free(buff);
	}
	else if (ret == -1)
		ftp_error(NULL, "read failure\n");*/
}

void			ftp_is_cmd(char *cmd)
{
	if (!ft_strcmp(cmd, "pwd"))
		;
	else if (!ft_strncmp(cmd, "ls", 2))
		;
	else if (!ft_strncmp(cmd, "cd", 2))
		;
	else if (!ft_strncmp(cmd, "get ", 4))
		;
	else if (!ft_strncmp(cmd, "put ", 4))
		;
	else if (!ft_strcmp(cmd, "quit"))
		;
	else
		ft_putstr("Unknow command !\n");
}

void			ftp_get_stdin(void)
{
	ssize_t	ret;
	char		*line;

	ft_putendl("gnl");
	if ((ret = gnl(0, &line) > 0))
	{
		ft_putendl(line);
		ftp_is_cmd(ft_strtrim(line));
	}
	else if (ret == -1)
		ftp_error(NULL, "gnl failure\n");
}

int			main(int ac, char *av[])
{
	int					port;
	int					sock;
	t_header	header;

	if (ac != 3)
		ftp_error("Usage: s% <addr> <port>\n", av[0]);
	if (!(port = ft_atoi(av[2])) && av[1][0] != '0')
		ftp_error(NULL, "Invalid port !\n");
	sock = ftp_create_client(av[1], port);
	while (1)
	{
			  ft_putendl("wtf");
		ftp_get_server_package(sock, &header);
		ftp_get_stdin();
	}
	close(sock);
	return (0);
}
