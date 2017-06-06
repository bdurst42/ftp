/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/17 16:59:21 by bdurst            #+#    #+#             */
/*   Updated: 2017/06/05 21:09:41 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

char		**ftp_list_to_tabstr(t_list *list)
{
	t_list		*tmp;
	t_list		*to_free;
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
		to_free = tmp;
		tmp = tmp->next;
		ft_putendl("ici");
		free(to_free);
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
		if (args[i][ft_strlen(args[i]) - 1] == '/')
			args[i][ft_strlen(args[i]) - 1] = 0;
		arg->str = ft_strtrim(args[i]);
		arg->base = 1;
		ft_node_push_back(&list, arg);
	}
	ft_free_tab(args);
	return (list);
}

int			ftp_free_strjoin(char *s1, char **s2, char c)
{
	char	*tmp;

	tmp = *s2;
	if (c)
		*s2 = ft_strjoin(s1, *s2);
	else
		*s2 = ft_strjoin(*s2, s1);
	if (tmp)
		free(tmp);
	return (1);
}

static int	ftp_if_dot(char **current_path, char *path, int i)
{
	int		pos;
	int		j;
	char	*to_free;

	j = i;
	while (path[j] == '.')
	{
		if (j++ != i)
		{
			if (*current_path && (pos = ftp_find_last_directory(*current_path)))
			{
				to_free = *current_path;
				*current_path = ft_strsub(*current_path, 0,
				ft_strlen(*current_path) - pos);
				free(to_free);
			}
			else
			{
				free(*current_path);
				*current_path = NULL;
			}
		}
	}
	return (j);
}

char		*ftp_check_path(char *o_p, char *p)
{
	int		i;
	int		j;
	char	*c_p;

	c_p = getcwd(NULL, 0);
	if (ft_strncmp(p, o_p, ft_strlen(o_p)) && p[0] != '/'
		&& ftp_free_strjoin("/", &p, 1))
		ftp_free_strjoin(c_p, &p, 1);
	else if (ft_strncmp(p, o_p, ft_strlen(o_p)))
		ftp_free_strjoin(c_p, &p, 1);
	free(c_p);
	c_p = p;
	p = ft_strsub(p, ft_strlen(o_p), ft_strlen(p) - ft_strlen(o_p));
	free(c_p);
	c_p = NULL;
	i = -1;
	while (p[++i])
	{
		if (p[i] == '/')
			continue ;
		j = i;
		if (p[j] && p[j++] != '.' && (j = ft_strfind(p, j, '/')))
			ftp_free_strjoin(ft_strsub(p, i - 1, j - i + 1), &c_p, 0);
		else
			j = ftp_if_dot(&c_p, p, i);
		i = j;
	}
	ftp_free_strjoin(o_p, &c_p, 1);
	return (c_p);
}
