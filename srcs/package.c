/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   package.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 16:22:24 by bdurst            #+#    #+#             */
/*   Updated: 2017/06/06 03:24:29 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

int		ftp_is_file(char *path)
{
	struct stat path_stat;

	if (stat(path, &path_stat) != 0)
		return (0);
	return (S_ISREG(path_stat.st_mode));
}

char	*ftp_get_package(int sock, t_header *header)
{
	char	*buff;
	ssize_t	ret;

	ret = 0;
	while ((ret += recv(sock, header, sizeof(t_header) - ret, 0)) >= 0
				&& (size_t)ret < sizeof(t_header))
		;
	if (ret != -1)
	{
		if (!(buff = (char*)malloc(header->nb_bytes + 1)))
			ftp_error(NULL, "ERROR: malloc failure\n", 0);
		ret = 0;
		while ((ret += recv(sock, buff + ret, header->nb_bytes - ret, 0)) >= 0
				&& (size_t)ret < header->nb_bytes)
			;
		if (ret > 0)
		{
			buff[ret] = '\0';
			return (buff);
		}
	}
	if (ret == -1)
		ftp_error(NULL, "ERROR: recv failure\n", 0);
	return (NULL);
}

void	ftp_send_package(char *str, int sock, char flag, long size)
{
	t_header	header;
	ssize_t		ret;
	char		*package;
	int			size_header;

	if (size == -1)
		header.nb_bytes = ft_strlen(str);
	else
		header.nb_bytes = size;
	header.flag = flag;
	size_header = sizeof(t_header);
	if (!(package = malloc(size_header + header.nb_bytes)))
		ftp_error(NULL, "ERROR: malloc failure !\n", sock);
	ft_memcpy(package, &header, size_header);
	ft_memcpy(package + size_header, str, header.nb_bytes);
	if ((ret = send(sock, package, size_header + header.nb_bytes, 0)) == -1)
		ftp_error(NULL, "ERROR: send failure\n", sock);
	free(package);
}

void	ftp_error(char *format_string, char *str, int sock)
{
	if (format_string)
		printf(format_string, str);
	else
		ft_putstr(str);
	if (sock)
		close(sock);
	exit(-1);
}
