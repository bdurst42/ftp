/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nmatch.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/09/07 22:39:27 by bdurst            #+#    #+#             */
/*   Updated: 2017/06/04 01:25:03 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

int			nmatch(char *s1, char *s2)
{
	if (*s2 == '*' && *s1 == '\0')
		return (nmatch(s1, s2 + 1));
	if (*s2 == '*' && *s1 != '\0')
		return (nmatch(s1 + 1, s2) + nmatch(s1, s2 + 1));
	if (*s1 == '\0' && *s2 == '\0' && *s2 == *s1)
		return (1);
	if (*s1 != '\0' && *s2 != '\0' && *s2 == *s1)
		return (nmatch(s1 + 1, s2 + 1));
	return (0);
}

char		*ftp_get_stdin(int sock)
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

static void	ftp_open_file(char *file, int sock, char flag)
{
	if (flag & F_CLIENT)
		printf("ERROR: open failure -> %s\n", file);
	else
		ftp_send_package(ft_strjoin("ERROR: open failure -> ", file),
				sock, 2, -1);
}

void		ftp_send_file(char *file, int sock, char flag)
{
	int		fd;
	char	buff[PACKAGE_SIZE + 1];
	ssize_t	ret;
	long	len;

	ft_putstr("file ==== ");
	ft_putendl(file);
	if ((fd = open(file, O_RDONLY)) == -1)
		ftp_open_file(file, sock, flag);
	else
	{
		len = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		while ((ret = read(fd, buff, PACKAGE_SIZE)) > 0 || len > 0)
		{
			ft_putnbr(len);
			ft_putendl(" <--------------------------------------------------- len");
			len -= PACKAGE_SIZE;
			buff[ret] = '\0';
			flag |= F_FILE_NO_END;
			ft_putnbr(ret);
			ft_putendl(" <-- ret send");
			ft_putnbr(flag);
			ft_putendl(" <-- flag");
			ftp_send_package(buff, sock, flag, ret);
		}
		close(fd);
		ftp_send_package("", sock, 2, 0);
		ft_putendl("FIRST");
	}
}
