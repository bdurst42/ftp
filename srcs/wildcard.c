/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 17:38:05 by bdurst            #+#    #+#             */
/*   Updated: 2017/06/05 20:24:05 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

static void	ftp_manage_stars(char *path, t_list **list, char *end_path)
{
	DIR				*dir;
	struct dirent	*ent;
	char			*dir_path;
	t_arg			*arg;
	char			*join;

	dir_path = ft_strsub(path, 0, ftp_find_last_directory(path));
	if ((dir = ftp_opendir(dir_path, -2)))
	{
		while ((ent = readdir(dir)) != NULL)
		{
			join = ft_strjj(dir_path, "/", ent->d_name);
			if (ent->d_name[0] != '.' && (ftp_is_dir(join) || !end_path)
			&& nmatch(ent->d_name, path + ftp_find_last_directory(path) + 1))
			{
				if (end_path)
				{
					if (!(arg = (t_arg*)malloc(sizeof(t_arg))))
						ftp_error(NULL, "ERROR: malloc failure !", 0);
					arg->str = ft_strjoin(join, end_path);
					arg->base = 0;
					ft_node_push_after(list, arg);
				}
				else
					ft_node_push_back(list, join);
			}
		}
		closedir(dir);
	}
	free(dir_path);
	free(path);
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
	char	*sub;
	char	*c_p;

	sub = ft_strsub(arg, 0, pos);
	if (path)
		dir_path = ftp_check_path(path, sub);
	else
	{
		c_p = getcwd(NULL, 0);
		if (!ft_strstr(arg, c_p))
			dir_path = ft_strjj(c_p, "/", sub);
		else
			dir_path = sub;
		free(c_p);
	}
	if (dir_path != sub)
		free(sub);
	return (dir_path);
}

void		ftp_wildcards(t_list **args, t_list **list, char *path)
{
	int		pos;
	char	*dir_path;
	int		i;
	t_list	*tmp;
	char	*arg;
	char	*c_p;

	i = -1;
	tmp = *args;
	arg = ((t_arg*)tmp->data)->str;
	c_p = getcwd(NULL, 0);
	if (!(pos = ftp_find_last_directory(arg)) && arg[0] != '/')
		ftp_manage_stars(ft_strjj(c_p, "/", arg),
		list, 0);
	else
		while (arg[++i])
			if (arg[i] == '*')
			{
				pos = ftp_get_current_sf(arg, i);
				dir_path = ftp_get_dir_path(path, arg, pos);
				if (arg[pos] == '/')
					ftp_manage_stars(dir_path, args, arg + pos);
				else
					ftp_manage_stars(dir_path, list, NULL);
				break ;
			}
		ft_putendl("step6");
	free(c_p);
}
