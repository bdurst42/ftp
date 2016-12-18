/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/17 16:59:21 by bdurst            #+#    #+#             */
/*   Updated: 2016/12/17 18:40:31 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

int		ftp_find_last_directory(char *path)
{
	int i;

	i = ft_strlen(path) - 1;
	while (i > 0 && path[i] != '/')
		--i;
	return (i);
}

static t_list	*ftp_manage_stars(char *dir_name, t_list *list, char *path,
			int c_sock)
{
	DIR				*dir;
	struct dirent	*ent;

	if ((dir = ftp_opendir(dir_name, c_sock)))
	{
		while ((ent = readdir(dir)) != NULL)
			if (ent->d_name[0] != '.' &&
			nmatch(ent->d_name, path + ftp_find_last_directory(path)))
				ft_node_push_back(&list, ft_strjoin(dir_name, ent->d_name));
		closedir(dir);
	}
	return (list);
}

static void			ftp_wildcards(char *arg, int c_sock, t_list **list, char *path)
{
	int		pos;
	char	*dir;

	if (!(pos = ftp_find_last_directory(ft_strtrim(arg))))
		*list = ftp_manage_stars(ft_strjoin(getcwd(NULL, 0), "/"), *list,
		ft_strtrim(arg), c_sock);
	else
	{
		dir = ftp_check_path(path, ft_strsub(ft_strtrim(arg), 0, pos));
		if (ftp_is_dir(dir))
			*list = ftp_manage_stars(ft_strjoin(dir, "/"), *list,
			ft_strtrim(arg), c_sock);
	}
}

t_list				*ftp_get_args(char **args, char opt, char *path, int c_sock)
{
	int		i;
	t_list	*list;

	i = -1;
	list = NULL;
	while (args[++i])
	{
		if (!i || !path)
			ft_node_push_back(&list, ft_strtrim(args[i]) +
			ftp_find_last_directory(ft_strtrim(args[i])));
		else if (!opt || args[i][0] != '-')
		{
			if (ft_strchr(args[i], '*'))
				ftp_wildcards(args[i], c_sock, &list, path);
			else
				ft_node_push_back(&list, ftp_check_path(path,
							ft_strtrim(args[i])));
		}
		else
			ft_node_push_back(&list, ft_strtrim(args[i]));
	}
	return (list);
}
