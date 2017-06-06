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

	ft_putendl("PACKAGE");
	if ((ret = recv(sock, header, sizeof(t_header), 0)) > 0)
	{
	/*ft_putnbr(ret);
	ft_putstr(" || ");
	ft_putnbr(header->nb_bytes);
	ft_putstr(" || ");
	ft_putnbr(header->flag);
	ft_putendl("WTF");
	ft_putnbr(sock);
	ft_putendl(" === sock");*/
	printf("%p\n", header);
		if (!(buff = (char*)malloc(sizeof(char) * (header->nb_bytes + 1))))
			ftp_error(NULL, "ERROR: malloc failure\n", 0);
		if ((ret = recv(sock, buff, header->nb_bytes, 0)) > 0)
		{
			buff[ret] = '\0';
			return (buff);
		}
		else if (!ret)
			ft_putendl("RET = 0 {{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{");
	}
	if (ret == -1)
		ftp_error(NULL, "ERROR: recv failure\n", 0);
	ft_putendl("OUT PACKAGE");
	return (NULL);
}

void	ftp_send_package(char *str, int sock, char flag, long size)
{
	t_header	header;

	//if (str)
	//	ft_putendl(str);
	if (size == -1)
	{
		header.nb_bytes = ft_strlen(str);
		ft_putendl("---------------------------> size = -1");
	}
	else
		header.nb_bytes = size;
	/*ft_putnbr(header.nb_bytes);
	ft_putendl(" _> nb bytes");
	ft_putnbr(sizeof(t_header));
	ft_putendl(" _> sizeof");
	*/header.flag = flag;
	/*ft_putnbr(header.flag);
	ft_putendl(" === flag");
	ft_putnbr(sock);
	ft_putendl(" === sock");*/
	if ((send(sock, &header, sizeof(t_header), 0)) == -1)
		ftp_error(NULL, "ERROR: send failure\n", sock);
	ft_putnbr(header.nb_bytes);
	ft_putendl(" AFTERRRRRRRRR nb bytes");
	if (header.nb_bytes && (send(sock, str, header.nb_bytes, 0)) == -1)
		ftp_error(NULL, "ERROR: send failure\n", sock);
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
