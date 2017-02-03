/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/17 16:59:21 by bdurst            #+#    #+#             */
/*   Updated: 2017/01/06 14:47:32 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

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

t_list		*ftp_tabstr_to_list(char **args)
{
	t_list	*list;
	t_arg	*arg;
	int		i;

	i = -1;
	list = NULL;
	while (args[++i])
	{
		if (!(arg = (t_arg*)malloc(sizeof(t_arg))))
			ftp_error(NULL, "ERROR: Malloc failure !", 0);
		arg->str = ft_strtrim(args[i]);
		arg->base = 1;
		ft_node_push_back(&list, arg);
	}
	return (list);
}

static void	ftp_free_strjoin(char *s1, char **s2, char c)
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

static int	ftp_if_dot(char **current_path, char *path, int i)
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
			j = ft_strfind(path, j, '/');
			ftp_free_strjoin(ft_strsub(path, i - 1, j - i + 1), &c_p, 0);
		}
		else
			j = ftp_if_dot(&c_p, path, i);
		i = j;
	}
	if (!c_p || !ft_strstr(c_p, o_p))
		ftp_free_strjoin(o_p, &c_p, 1);
	return (c_p);
}