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

int			ftp_find_last_directory(char *path)
{
	int i;

	i = ft_strlen(path) - 1;
	while (i > 0 && path[i] != '/')
		--i;
	return (i);
}

t_list		*ftp_manage_stars(char *dir_name, t_list *list, char *path,
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

char		**ftp_list_to_tabstr(t_list *list)
{
	t_list		*tmp;
	char		**args;
	uint32_t	size;
	uint32_t	i;

	tmp = list;
	size = ft_list_size(list);
	if (!(args = (char**)malloc(sizeof(char*) * (size + 1))))
		ftp_error(NULL, "ERROR: malloc failure\n", 0);
	args[size] = NULL;
	i = 0;
	while (tmp)
	{
		args[i++] = (char*)tmp->data;
		tmp = tmp->next;
	}
	return (args);
}

void		ftp_wildcards(char *arg, int c_sock, t_list **list, char *path)
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

t_list		*ftp_get_args(char **args, char opt, char *path, int c_sock)
{
	int		i;
	t_list	*list;

	i = -1;
	list = NULL;
	while (args[++i])
	{
		ft_putendl(args[i]);
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

void		ftp_free_strjoin(char *s1, char **s2, char c)
{
	char	*tmp;

	tmp = *s2;
	if (c)
		*s2 = ft_strjoin(s1, *s2);
	else
		*s2 = ft_strjoin(*s2, s1);
	if (tmp)
		free(tmp);
}

int			ftp_if_dot(char **current_path, char *path, int i)
{
	int	pos;
	int	j;

	j = i;
	while (path[j] == '.')
	{
		if (j++ != i)
		{
			if (*current_path && (pos = ftp_find_last_directory(*current_path)))
				*current_path = ft_strsub(*current_path, 0,
				ft_strlen(*current_path) - pos);
			else
			{
				free(*current_path);
				*current_path = NULL;
			}
		}
	}
	return (j);
}

char		*ftp_check_path(char *o_p, char *path)
{
	int		i;
	int		j;
	char	*c_p;

	c_p = ft_strdup(ft_strstr(getcwd(NULL, 0), o_p) + ft_strlen(o_p));
	if (path && path[0] != '/')
		ftp_free_strjoin("/", &path, 1);
	i = -1;
	while (path[++i])
	{
		if (path[i] == '/')
			continue ;
		j = i;
		if (path[j] && path[j++] != '.')
		{
			while (path[j] && path[j] != '/')
				++j;
			ftp_free_strjoin(ft_strsub(path, i - 1, j - i + 1), &c_p, 0);
		}
		else
			j = ftp_if_dot(&c_p, path, i);
		i = j;
	}
	ftp_free_strjoin(o_p, &c_p, 1);
	return (c_p);
}
