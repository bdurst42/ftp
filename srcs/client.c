/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 15:57:22 by bdurst            #+#    #+#             */
/*   Updated: 2017/02/02 16:53:46 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

volatile int	g_sock = -1;

static int	ftp_create_client(char *addr, char *port)
{
	int				sock;
	struct addrinfo	info;
	struct addrinfo	*res;

	ft_memset(&info, 0, sizeof(info));
	info.ai_family = AF_UNSPEC;
	info.ai_socktype = SOCK_STREAM;
	info.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo(addr, port, &info, &res))
	{
		free(res);
		ftp_error(NULL, "ERROR: getaddrinfo failure !\n", 0);
	}
	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if ((connect(sock, res->ai_addr, res->ai_addrlen)) == -1)
	{
		free(res);
		ftp_error(NULL, "ERROR: connect failure !\n", 0);
	}
	free(res);
	return (sock);
}

static char	*ftp_get_stdin(int sock)
{
	ssize_t	ret;
	char	*line;

	ft_putstr("\033[0;34mftp\033[0m \033[0;32m->\033[0m ");
	if ((ret = gnl(0, &line) > 0))
		return (ft_strtrim(line));
	else if (ret == -1)
		ftp_error(NULL, "ERROR: gnl failure\n", sock);
	return (NULL);
}

static void	ftp_parse_cmd(char *cmd, int sock)
{
	t_list	*list;

	if (!ft_strncmp(cmd, "put ", 4))
	{
		ft_putendl("CLIENT");
		if ((list = ftp_get_args(ftp_tabstr_to_list(ft_strsplit(cmd, ' ')),
			0, NULL)))
		{
			ftp_send_package(cmd, sock, 0, -1);
			ftp_manage_send_cmd(cmd, list->next, sock, 1 + F_CLIENT);
		}
	}
	else
		ftp_send_package(cmd, sock, 0, -1);
}

static char	ftp_ret_cmd(char *cmd, int sock)
{
	t_list	*list;

	if (!ft_strcmp(cmd, "quit"))
	{
		close(sock);
		return (0);
	}
	else if (!ft_strcmp(cmd, "ls"))
		ftp_get_file(NULL, sock, 1);
	else if (!ft_strncmp(cmd, "get ", 4))
	{
		ft_putendl("CLIENT");
		if ((list = ftp_get_args(ftp_tabstr_to_list(ft_strsplit(cmd, ' ')),
			0, NULL)))
			ftp_manage_get_cmd(list->next, sock, 1);
	}
	else
		ft_putendl(cmd);
	return (1);
}

static void	sigsegv_handle(int n)
{
	(void)n;
	close(g_sock);
	exit(0);
}

int			main(int ac, char *av[])
{
	char		*cmd;
	t_header	header;

	if (ac != 3)
		ftp_error("Usage: %s <addr> <port>\n", av[0], 0);
	if ((g_sock = ftp_create_client(av[1], av[2])) == -1)
		return (-1);
	signal(SIGINT, sigsegv_handle);
	while (1)
	{
		header.flag = 2;
		while (header.flag & F_CONTINUE &&
				(cmd = ftp_get_package(g_sock, &header)))
			if (!(ftp_ret_cmd(cmd, g_sock)))
				return (0);
		while (!(cmd = ftp_get_stdin(g_sock)) || !cmd[0])
			;
		ftp_parse_cmd(cmd, g_sock);
	}
	close(g_sock);
	return (0);
}
