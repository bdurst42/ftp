/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_files.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 18:10:44 by bdurst            #+#    #+#             */
/*   Updated: 2017/06/05 16:57:45 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

static void	ftp_manage_file(char *path, t_tools t, char *real_path)
{
	char	*to_free;

	if (t.cmd && !(IS_CLIENT))
		ftp_send_package(t.cmd, t.sock, 0, -1);
	to_free = ft_strjoin("mkfile ", path + ft_strlen(real_path));
	ftp_send_package(to_free, t.sock, 2, -1);
	free(to_free);
	ftp_send_file(path, t.sock, t.flag);
}

static void	ftp_manage_dir(char *dir, char *p, t_tools t, char *r_p)
{
	t_list	*n;
	char	*to_free;

	if (t.cmd && !(IS_CLIENT))
		ftp_send_package(t.cmd, t.sock, 0, -1);
	if ((IS_CLIENT && (n = ftp_get_sf_in_dir(dir, -1)) == (void*)-1) ||
	(!(IS_CLIENT) && (n = ftp_get_sf_in_dir(dir, t.sock)) == (void*)-1))
	{
		ftp_send_package("", t.sock, 2, -1);
		return ;
	}
	to_free = ft_strjoin("mkdir ", p + ft_strlen(r_p));
	ftp_send_package(to_free, t.sock, 2, -1);
	free(to_free);
	t.cmd = NULL;
	ftp_manage_send_cmd(n, t, r_p);
}

static void	ftp_manage_sf(t_list **list, t_tools t, char *real_path)
{
	char	*path;
	char	*c_p;

	c_p = getcwd(NULL, 0);
	if (ft_strstr((char*)(*list)->data, c_p))
		path = (char*)(*list)->data + ft_strlen(c_p) + 1;
	else
		path = (char*)(*list)->data;
	free(c_p);
	if (ftp_is_dir((char*)(*list)->data))
		ftp_manage_dir((char*)(*list)->data, path, t, real_path);
	else if (ftp_is_file((char*)(*list)->data))
		ftp_manage_file(path, t, real_path);
	else
	{
		c_p = ft_strjoin("ERROR: No such file or directory -> ",
		(char*)(*list)->data);
		if (t.flag & F_CLIENT)
			printf("%s\n", c_p);
		else
			ftp_send_package(c_p, t.sock, 2, -1);
		free(c_p);
	}
}

static char	*ftp_get_real_path(char *path, int w_p)
{
	if (!ftp_find_last_directory(path) && path[0] != '/')
		w_p = 0;
	return (ft_strsub(path, 0, ftp_find_last_directory(path) + w_p));
}

void		ftp_manage_send_cmd(t_list *list, t_tools t, char *r_p)
{
	int		w_p;
	char	*c_p;

	t.flag |= F_CONTINUE;
	c_p = getcwd(NULL, 0);
	while (list)
	{
		w_p = 1;
		if (t.cmd && ft_strstr(PATH, c_p))
			r_p = ftp_get_real_path(ARG, w_p);
		else if (t.cmd)
			r_p = ftp_get_real_path(PATH, w_p);
		ftp_manage_sf(&list, t, r_p);
		if (r_p && t.cmd)
			free(r_p);
		list = list->next;
	}
	if (t.cmd)
		ftp_send_package("", t.sock, 0, -1);
}
