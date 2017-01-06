#include "ftp.h"

int		ftp_find_last_directory(char *path)
{
	int i;

	i = ft_strlen(path) - 1;
	while (i > 0 && path[i] != '/')
		--i;
	return (i);
}

static void		ftp_manage_stars(char *path, t_list **list, char *end_path)
{
	DIR				*dir;
	struct dirent	*ent;
	char				*dir_path;
	t_arg				*arg;

	dir_path = ft_strsub(path, 0, ftp_find_last_directory(path));
	ft_putendl(dir_path);
	if ((dir = ftp_opendir(dir_path, -2)))
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_name[0] != '.' && (ftp_is_dir(ft_strjoin(ft_strjoin(dir_path, "/"), ent->d_name)) || !end_path) &&
			nmatch(ent->d_name, path + ftp_find_last_directory(path) + 1))
			{
				if (end_path)
				{
					if (!(arg = (t_arg*)malloc(sizeof(t_arg))))
							  ftp_error(NULL, "ERROR: malloc failure !", 0);
					arg->str = ft_strjoin(ft_strjoin(ft_strjoin(dir_path, "/"), ent->d_name), end_path);
					arg->base = 0;
					ft_node_push_after(list, arg);
				}
				else
					ft_node_push_back(list, ft_strjoin(ft_strjoin(dir_path, "/"), ent->d_name));
			}
		}
		closedir(dir);
	}
}

static int		ftp_get_current_sf(char *path, int pos)
{
	while (path[pos] && path[pos] != '/')
		++pos;
	return (pos);
}

static void		ftp_wildcards(t_list **args, t_list **list, char *path)
{
	int		pos;
	char	*dir_path;
	int		i;
	t_list *tmp;
	char	*arg;

	i = -1;
	tmp = *args;
	arg = ((t_arg*)tmp->data)->str;
	if (!(pos = ftp_find_last_directory(arg)))
		ftp_manage_stars(ft_strjoin(ft_strjoin(getcwd(NULL, 0), "/"), arg), list,
		0);
	else
	{
		while (arg[++i])
			if (arg[i] == '*')
			{
				pos = ftp_get_current_sf(arg, i);
				if (path)
					dir_path = ftp_check_path(path, ft_strsub(arg, 0, pos));
				else
				{
					if (!ft_strstr(arg, getcwd(NULL, 0)))
						dir_path = ft_strjoin(ft_strjoin(getcwd(NULL, 0), "/"), ft_strsub(arg, 0, pos));
					else
						dir_path = ft_strsub(arg, 0, pos);
				}
				if (arg[pos] == '/')
					ftp_manage_stars(dir_path, args, arg + pos);
				else
					ftp_manage_stars(dir_path, list, NULL);
				break ;
			}
	}
}

t_list				*ftp_get_args(t_list	*args, char opt, char *path)
{
	t_list	*list;
	char		*arg;
	char		is_opt;
	char		*last_base_arg;
	int		last_list_size;

	list = NULL;
	is_opt = 1;
	while (args)
	{
		arg = ((t_arg*)args->data)->str;
		if (!list || (!path && !ft_strchr(arg, '*')))
		{
			ft_node_push_back(&list, arg +
			ftp_find_last_directory(arg));
		}
		else if (!opt || !is_opt || arg[0] != '-')
		{
			if (!is_opt && last_base_arg && ((t_arg*)args->data)->base && last_list_size == ft_list_size(list))
				ft_node_push_back(&list, last_base_arg);
			is_opt = 0;
			if (ft_strchr(arg, '*'))
			{
				if (((t_arg*)args->data)->base)
				{
					last_base_arg = arg;
					last_list_size = ft_list_size(list);
				}
				ftp_wildcards(&args, &list, path);
			}
			else
				if (((t_arg*)args->data)->base || (ftp_is_dir(arg) || ftp_is_file(arg)))
					ft_node_push_back(&list, ftp_check_path(path,
					arg));
		}
		else
			ft_node_push_back(&list, arg);
		args = args->next;
	}
	if (last_base_arg && last_list_size == ft_list_size(list))
		ft_node_push_back(&list, last_base_arg);
	return (list);
}
