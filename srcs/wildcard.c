/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 17:38:05 by bdurst            #+#    #+#             */
/*   Updated: 2017/01/06 18:07:26 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

static void	ftp_manage_stars(char *path, t_list **list, char *end_path)
{
	DIR				*dir;
	struct dirent	*ent;
	char			*dir_path;
	t_arg			*arg;

	ft_putstr("path = ");
	ft_putendl(path);
	dir_path = ft_strsub(path, 0, ftp_find_last_directory(path));
	ft_putstr("dir_path = ");
	ft_putendl(dir_path);
	if ((dir = ftp_opendir(dir_path, -2)))
	{
		while ((ent = readdir(dir)) != NULL)
			if (ent->d_name[0] != '.' && (ftp_is_dir(JOIN) || !end_path)
			&& nmatch(ent->d_name, path + ftp_find_last_directory(path) + 1))
			{
				if (end_path)
				{
					if (!(arg = (t_arg*)malloc(sizeof(t_arg))))
						ftp_error(NULL, "ERROR: malloc failure !", 0);
					arg->str = ft_strjoin(JOIN, end_path);
					arg->base = 0;
					ft_node_push_after(list, arg);
				}
				else
					ft_node_push_back(list, JOIN);
			}
		closedir(dir);
	}
}

static int	ftp_get_current_sf(char *path, int pos)
{
	while (path[pos] && path[pos] != '/')
		++pos;
	return (pos);
}

static char	*ftp_get_dir_path(char *path, char *arg, int pos)
{
	char	*dir_path;

	if (path)
		dir_path = ftp_check_path(path, ft_strsub(arg, 0, pos));
	else
	{
		if (!ft_strstr(arg, getcwd(NULL, 0)))
			dir_path = ft_strjoin(ft_strjoin(getcwd(NULL, 0), "/"),
			ft_strsub(arg, 0, pos));
		else
			dir_path = ft_strsub(arg, 0, pos);
	}
	ft_putstr("dir_path ===> ");
	ft_putendl(dir_path);
	return (dir_path);
}

void		ftp_wildcards(t_list **args, t_list **list, char *path)
{
	int		pos;
	char	*dir_path;
	int		i;
	t_list	*tmp;
	char	*arg;

	i = -1;
	tmp = *args;
	arg = ((t_arg*)tmp->data)->str;
	ft_putstr("path = ");
	ft_putendl(path);
	ft_putstr("arg = ");
	ft_putendl(arg);
	if ((pos = ftp_find_last_directory(arg)) == -1)
		ftp_manage_stars(ft_strjoin(ft_strjoin(getcwd(NULL, 0), "/"), arg),
		list, 0);
	else
		while (arg[++i])
			if (arg[i] == '*')
			{
				pos = ftp_get_current_sf(arg, i);
				dir_path = ftp_get_dir_path(path, arg, pos);
				if (arg[pos] == '/')
				{
					ft_putendl("first");
					ftp_manage_stars(dir_path, args, arg + pos);
				}
				else
				{
					ft_putendl("second");
					ftp_manage_stars(dir_path, list, NULL);
				}
				break ;
			}
}
