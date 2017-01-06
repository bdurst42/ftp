/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   package.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 16:22:24 by bdurst            #+#    #+#             */
/*   Updated: 2017/01/06 16:23:31 by bdurst           ###   ########.fr       */
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

	if ((ret = recv(sock, header, sizeof(t_header), 0)) > 0)
	{
		if (!(buff = (char*)malloc(sizeof(char) * (MAX_PACKAGE_SIZE + 1))))
			ftp_error(NULL, "ERROR: malloc failure\n", 0);
		if ((ret = recv(sock, buff, header->nb_bytes, 0)) > 0)
		{
			buff[ret] = '\0';
			return (buff);
		}
	}
	else if (ret == -1)
		ftp_error(NULL, "ERROR: recv failure\n", 0);
	return (NULL);
}

void	ftp_send_package(char *str, int sock, char flag, long size)
{
	t_header	header;

	if (size == -1)
		header.nb_bytes = ft_strlen(str);
	else
		header.nb_bytes = size;
	header.flag = flag;
	if ((send(sock, &header, sizeof(t_header), 0)) == -1)
		ftp_error(NULL, "ERROR: send failure\n", 0);
	if ((send(sock, str, header.nb_bytes, 0)) == -1)
		ftp_error(NULL, "ERROR: send failure\n", 0);
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
