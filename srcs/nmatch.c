/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nmatch.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/09/07 22:39:27 by bdurst            #+#    #+#             */
/*   Updated: 2017/03/20 13:07:57 by bdurst           ###   ########.fr       */
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
	char	*buff;
	ssize_t	ret;
	int		len;

	if ((fd = open(file, O_RDONLY)) == -1)
		ftp_open_file(file, sock, flag);
	else
	{
		len = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		if (!(buff = (char*)malloc(sizeof(char) * (len + 1))))
			ft_exiterror("malloc failure", -1);
		while ((ret = read(fd, buff, len)) > 0)
		{
			buff[ret] = '\0';
			flag |= F_FILE_NO_END;
			ftp_send_package(buff, sock, flag, ret);
		}
		free(buff);
		close(fd);
		ftp_send_package("", sock, 2, -1);
	}
}
