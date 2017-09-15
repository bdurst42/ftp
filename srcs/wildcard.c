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

static void	ftp_multiple_wildcards(t_list **args, t_list **list, char *path,
			char *arg)
{
	char	*dir_path;
	int		i;
	int		pos;

	i = -1;	
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
}

void		ftp_wildcards(t_list **args, t_list **list, char *path)
{
	t_list	*tmp;
	char	*arg;
	char	*c_p;

	tmp = *args;
	arg = ((t_arg*)tmp->data)->str;
	c_p = getcwd(NULL, 0);
	if (!ftp_find_last_directory(arg) && arg[0] != '/')
		ftp_manage_stars(ft_strjj(c_p, "/", arg),
		list, 0);
	else
		ftp_multiple_wildcards(args, list, path, arg);
	free(c_p);
}
