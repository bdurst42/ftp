/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/17 12:36:17 by bdurst            #+#    #+#             */
/*   Updated: 2016/12/17 12:37:07 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

static int	ftp_create_server(char *port)
{
	int					sd;
	int					on;
	struct sockaddr_in6	serveraddr;

	if ((sd = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
		ftp_error(NULL, "socket failure", 0);
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0)
		ftp_error(NULL, "setsockopt failure", 0);
	ft_memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_port = htons(ft_atoi(port));
	serveraddr.sin6_addr = in6addr_any;
	if (bind(sd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
		ftp_error(NULL, "bind failure", 0);
	if (listen(sd, 512) < 0)
		ftp_error(NULL, "listen failure", 0);
	return (sd);
}

static void	ftp_fork(int c_sock)
{
	pid_t		pid;
	t_header	header;
	char		*path;

	path = getcwd(NULL, 0);
	if ((pid = fork()) == -1)
		ftp_send_package("ERROR: fork failure !", c_sock, 0, -1);
	else if (pid != 0)
	{
		ftp_send_package("SUCCES: connection", c_sock, 0, -1);
		while (ftp_is_cmd(ftp_get_package(c_sock, &header), c_sock, path))
			;
	}
}

int			main(int ac, char *av[])
{
	int	sock;
	int	c_sock;

	if (ac != 2)
		ftp_error("Usage: s% <port>\n", av[0], 0);
	if ((sock = ftp_create_server(av[1])) == -1)
		return (-1);
	while (1)
	{
		if ((c_sock = accept(sock, NULL, NULL)) == -1)
			ft_putstr("ERROR: accept failure !\n");
		ftp_fork(c_sock);
	}
	close(c_sock);
	close(sock);
	return (0);
}
