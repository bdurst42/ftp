/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_files.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 18:10:44 by bdurst            #+#    #+#             */
/*   Updated: 2017/03/20 13:28:23 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

static void	ftp_manage_file(char *cmd, char *path, t_tools t, char *real_path)
{
	if (cmd)
		ftp_send_package(ft_strjoin("mkfile ",
		path + ft_strlen(real_path)), t.sock, 2, -1);
	else
		ftp_send_package(ft_strjoin("mkfile ", path + ft_strlen(real_path)),
		t.sock, 2, -1);
	ftp_send_file(path, t.sock, t.flag);
}

static void	ftp_manage_dir(t_list *list, char *p, t_tools t, char *r_p)
{
	t_list	*n;

	if ((IS_CLIENT && (n = ftp_get_sf_in_dir(PATH, -1)) == (void*)-1) ||
	(!(IS_CLIENT) && (n = ftp_get_sf_in_dir(PATH, t.sock)) == (void*)-1))
	{
		ftp_send_package("", t.sock, 2, -1);
		return ;
	}
	ftp_send_package(ft_strjoin("mkdir ", p + ft_strlen(r_p)), t.sock, 2, -1);
	ftp_manage_send_cmd(NULL, n, t, r_p);
}

static void	ftp_manage_sf(char *cmd, t_list **list, t_tools t, char *real_path)
{
	char	*path;

	if (ft_strstr((char*)(*list)->data, getcwd(NULL, 0)))
		path = (char*)(*list)->data + ft_strlen(getcwd(NULL, 0)) + 1;
	else
		path = (char*)(*list)->data;
	if (ftp_is_dir((char*)(*list)->data))
		ftp_manage_dir(*list, path, t, real_path);
	else if (ftp_is_file((char*)(*list)->data))
		ftp_manage_file(cmd, path, t, real_path);
	else
	{
		if (t.flag & F_CLIENT)
			printf("ERROR: No such file or directory -> %s\n",
					(char*)(*list)->data);
		else
			ftp_send_package(ft_strjoin("ERROR: No such file or directory -> ",
						(char*)(*list)->data), t.sock, 2, -1);
	}
}

void		ftp_manage_send_cmd(char *cmd, t_list *list, t_tools t, char *r_p)
{
	int	w_p;

	t.flag |= F_CONTINUE;
	while (list)
	{
		w_p = 1;
		if (cmd && ft_strstr(PATH, getcwd(NULL, 0)))
		{
			if (!ftp_find_last_directory(ARG) && (ARG)[0] != '/')
				w_p = 0;
			r_p = ft_strsub(ARG, 0, ftp_find_last_directory(ARG) + w_p);
		}
		else if (cmd)
		{
			if (!ftp_find_last_directory(PATH) && (PATH)[0] != '/')
				w_p = 0;
			r_p = ft_strsub(PATH, 0, ftp_find_last_directory(PATH) + w_p);
		}
		ftp_manage_sf(cmd, &list, t, r_p);
		list = list->next;
	}
	if (cmd)
		ftp_send_package("", t.sock, 0, -1);
}
