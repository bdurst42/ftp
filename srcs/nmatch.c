/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nmatch.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/09/07 22:39:27 by bdurst            #+#    #+#             */
/*   Updated: 2017/06/06 03:30:21 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

char		*ftp_free_strtrim(char *str)
{
	char	*to_free;

	if (str)
	{
		to_free = str;
		str = ft_strtrim(str);
		free(to_free);
	}
	return (str);
}

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

	line = NULL;
	ft_putstr("\033[0;34mftp\033[0m \033[0;32m->\033[0m ");
	if ((ret = gnl(0, &line) > 0))
		return (ftp_free_strtrim(line));
	else if (ret == -1)
		ftp_error(NULL, "ERROR: gnl failure\n", sock);
	return (NULL);
}

static void	ftp_open_file(char *file, int sock, char flag)
{
	char	*to_free;

	to_free = ft_strjoin("ERROR: open failure -> ", file);
	if (flag & F_CLIENT)
		printf("%s\n", to_free);
	else
		ftp_send_package(to_free, sock, 2, -1);
	free(to_free);
}

void		ftp_send_file(char *file, int sock, char flag)
{
	int		fd;
	char	buff[PACKAGE_SIZE + 1];
	ssize_t	ret;

	if ((fd = open(file, O_RDONLY)) == -1)
		ftp_open_file(file, sock, flag);
	else
	{
		while ((ret = read(fd, buff, PACKAGE_SIZE)) > 0)
		{
			buff[ret] = '\0';
			flag |= F_FILE_NO_END;
			ftp_send_package(buff, sock, flag, ret);
		}
		close(fd);
		ftp_send_package("", sock, 2, 0);
	}
}
