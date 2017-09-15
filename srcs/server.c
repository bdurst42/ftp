/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/17 12:36:17 by bdurst            #+#    #+#             */
/*   Updated: 2017/06/05 21:06:28 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"
#include <signal.h>

volatile int g_c_sock = -1;
volatile int g_sock = -1;

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
	char		*cmd;

	path = getcwd(NULL, 0);
	if ((pid = fork()) == -1)
		ftp_send_package("ERROR: fork failure !", c_sock, 0, -1);
	else if (pid != 0)
	{
		ftp_send_package("SUCCES: connection", c_sock, 0, -1);
		cmd = ftp_get_package(c_sock, &header);
		while (cmd && ftp_is_cmd(cmd, c_sock, path))
		{
			free(cmd);
			cmd = ftp_get_package(c_sock, &header);
		}
		free(cmd);
	}
	free(path);
}

static void	sigsegv_handle(int n)
{
	(void)n;
	if (g_c_sock != -1)
		close(g_c_sock);
	close(g_sock);
	exit(n);
}

int			main(int ac, char *av[])
{
	if (ac != 2)
		ftp_error("Usage: %s <port>\n", av[0], 0);
	if ((g_sock = ftp_create_server(av[1])) == -1)
		return (-1);
	signal(SIGINT, sigsegv_handle);
	while (1)
	{
		if ((g_c_sock = accept(g_sock, NULL, NULL)) == -1)
			ft_putstr("ERROR: accept failure !\n");
		ftp_fork(g_c_sock);
	}
	if (g_c_sock != -1)
		close(g_c_sock);
	close(g_sock);
	return (0);
}
