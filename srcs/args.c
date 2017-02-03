/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 16:24:10 by bdurst            #+#    #+#             */
/*   Updated: 2017/01/06 17:54:37 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

int			ftp_find_last_directory(char *path)
{
	int i;

	i = ft_strlen(path) - 1;
	while (i >= 0 && path[i] != '/')
		--i;
	return (i);
}

static void	ftp_manage_sf(t_list **args, char *path, t_list **list,
							t_last *last)
{
	t_arg *arg;

	arg = (t_arg*)(*args)->data;
	if (!last->is_opt && last->base_arg && arg->base &&
		last->list_size == ft_list_size(*list))
	{
		ft_putendl("one");
		ft_node_push_back(list, last->base_arg);
	}
	last->is_opt = 0;
	if (ft_strchr(arg->str, '*'))
	{
		ft_putendl("two");
		if (arg->base)
		{
			last->base_arg = arg->str;
			last->list_size = ft_list_size(*list);
		}
		ftp_wildcards(args, list, path);
	}
	else
	{
		ft_putendl("three");
		ft_putendl(arg->str);
		ft_putendl(path);
		ft_putendl(ftp_check_path(path, arg->str));
		if (arg->base || (ftp_is_dir(arg->str) || ftp_is_file(arg->str)))
			ft_node_push_back(list, ftp_check_path(path, arg->str));
	}
}

t_list		*ftp_get_args(t_list *args, char opt, char *path)
{
	t_list	*list;
	char	*arg;
	t_last	last;

	list = NULL;
	last.is_opt = 1;
	while (args)
	{
		arg = ((t_arg*)args->data)->str;
		ft_putstr("arg in main -> ");
		ft_putendl(arg);
		if (!list || (!path && !ft_strchr(arg, '*')))
		{
			ft_putendl("step1");
			ft_node_push_back(&list, arg + ftp_find_last_directory(arg));
		}
		else if (!opt || !last.is_opt || arg[0] != '-')
		{
			ft_putendl("step2");
			ftp_manage_sf(&args, path, &list, &last);
		}
		else
		{
			ft_putendl("step3");
			ft_node_push_back(&list, arg);
		}
		args = args->next;
	}
	if (last.base_arg && last.list_size == ft_list_size(list))
		ft_node_push_back(&list, last.base_arg);
	t_list *tmp;
	ft_putendl("LIST :");
	tmp = list;
	while (tmp)
	{
		ft_putendl((char*)tmp->data);
		tmp = tmp->next;
	}
	return (list);
}