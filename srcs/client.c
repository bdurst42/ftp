/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 15:57:22 by bdurst            #+#    #+#             */
/*   Updated: 2017/06/06 03:30:03 by bdurst           ###   ########.fr       */
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
		freeaddrinfo(res);
		ftp_error(NULL, "ERROR: getaddrinfo failure !\n", 0);
	}
	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if ((connect(sock, res->ai_addr, res->ai_addrlen)) == -1)
	{
		freeaddrinfo(res);
		ftp_error(NULL, "ERROR: connect failure !\n", 0);
	}
	freeaddrinfo(res);
	return (sock);
}

static void	ftp_parse_cmd(char *cmd, int sock)
{
	t_list	*list;
	t_tools	t;

	if (!ft_strncmp(cmd, "put ", 4))
	{
		if ((list = ftp_get_args(ftp_tabstr_to_list(ft_strsplit(cmd, ' ')),
			0, NULL)))
		{
			t.sock = sock;
			t.flag = 1 + F_CLIENT;
			ftp_send_package(cmd, sock, 0, -1);
			ftp_manage_send_cmd(cmd, list->next, t, NULL);
		}
		ft_clear_list(&list, (void*)&ftp_clear_list);
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
		free(cmd);
		return (0);
	}
	else if (!ft_strcmp(cmd, "ls"))
		ftp_get_file(NULL, sock, 1);
	else if (!ft_strncmp(cmd, "get ", 4))
	{
		if ((list = ftp_get_args(ftp_tabstr_to_list(ft_strsplit(cmd, ' ')),
			0, NULL)))
			ftp_manage_get_cmd(list->next, sock, 1);
	}
	else
		ft_putendl(cmd);
	free(cmd);
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
		{
			ft_putendl("get lien fail");
			if (cmd)
				free(cmd);
		}
		printf("%p | %s\n", cmd, cmd);
		ftp_parse_cmd(cmd, g_sock);
		printf("%p | %s\n", cmd, cmd);
		free(cmd);
	}
	close(g_sock);
	return (0);
}
