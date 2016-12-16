#include "ftp.h"

int             ftp_find_last_directory(char *path)
{
	int i;

	i = ft_strlen(path) - 1;
	while (i > 0 && path[i] != '/')
		--i;
	//ft_putnbr(i);
	//ft_putstr("\n");
	return (i);
}

char				*ftp_get_file_name(char *file_path)
{
	int	len;

	len = ft_strlen(file_path) - 1;
	while (len > -1 && file_path[len] != '/')
		--len;
	return (file_path + len + 1);
}

int					ftp_is_dir(char *dir)
{
	struct stat statbuf;

	if (stat(dir, &statbuf) != 0)
		return (0);
	return S_ISDIR(statbuf.st_mode);
}

DIR					*ftp_opendir(char *dir_name, int c_sock)
{
	DIR				*dir;

	if (!(dir = opendir(dir_name)))
	{
		ftp_send_package("ERROR: opendir failure", c_sock, 0, -1);
		return (NULL);
	}
	return (dir);
}

t_list				*ftp_manage_stars(char *dir_name, t_list *list, char *path, int c_sock)
{
	DIR				*dir;
	struct dirent	*ent;

	if ((dir = ftp_opendir(dir_name, c_sock)))
	{
		//ft_putstr("path ================ ");
		//ft_putendl(ftp_get_file_name(path));
		while ((ent = readdir (dir)) != NULL)
		{
			ft_putendl(ent->d_name);
			if (ent->d_name[0] != '.' && nmatch(ent->d_name, ftp_get_file_name(path)))
			{
				ft_putendl("enter");
				ft_node_push_back(&list, ft_strjoin(dir_name, ent->d_name));
			}
		}
		closedir (dir);
	}
	return (list);
}

char				**ftp_list_to_tabstr(t_list *list)
{
	t_list		*tmp;
	char		**args;
	uint32_t	size;
	uint32_t	i;

	tmp = list;
	size = ft_list_size(list);
	ft_putstr("\n");
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

t_list				*ftp_get_args(char **args, char opt, char *path, int c_sock)
{
	int		i;
	t_list	*list;
	int		pos;
	char	*dir;

	i = -1;
	list = NULL;
	while (args[++i])
	{
		ft_putendl(args[i]);
		if (!i || !path)
			ft_node_push_back(&list, ftp_get_file_name(ft_strtrim(args[i])));
		else if (!opt || args[i][0] != '-')
		{
			if (ft_strchr(args[i], '*'))
			{
				//ft_putendl("******STARS******");
				if (!(pos = ftp_find_last_directory(ft_strtrim(args[i]))))
				{
				//	ft_putendl("pos 0");
					list = ftp_manage_stars(ft_strjoin(getcwd(NULL, 0), "/"), list, ft_strtrim(args[i]), c_sock);
				}
				else
				{
					dir = ftp_check_path(path, ft_strsub(ft_strtrim(args[i]), 0, pos));
					if (ftp_is_dir(dir))
						list = ftp_manage_stars(ft_strjoin(dir, "/"), list, ft_strtrim(args[i]), c_sock);
				}
			}
			else
				ft_node_push_back(&list, ftp_check_path(path, ft_strtrim(args[i])));
		}
		else
			ft_node_push_back(&list, ft_strtrim(args[i]));
//		ft_putendl(args[i]);
	}
	ft_putendl("ARGS:");
	t_list *tmp;
	tmp = list;
	while (tmp)
	{
		ft_putendl((char*)tmp->data);
		tmp = tmp->next;
	}
	ft_putendl("");
	return (list);
}

char            *ftp_check_path(char *old_path, char *path)
{
	int         i;
	int         j;
	int         pos;
	char        *current_path;
	char		*tmp;

	//ft_putstr("getcwd = ");
	//  ft_putendl(getcwd(NULL, 0));
	// ft_putstr("old_path = ");
	 // ft_putendl(old_path);
	 // ft_putstr("ft_strstr = ");
	  //ft_putendl(ft_strstr(getcwd(NULL, 0), old_path));
	current_path = ft_strdup(ft_strstr(getcwd(NULL, 0), old_path) + ft_strlen(old_path));
/*	if (current_path)
	{
	  ft_putstr("current_path = ");
	  ft_putendl(current_path);
	}
	  ft_putstr("path = ");
	  ft_putendl(path);*/
	if (path && path[0] != '/')
	{
//		ft_putendl("first");
		tmp = path;
		path = ft_strjoin("/", tmp);
		free(tmp);
	}
//	  ft_putstr("path = ");
//	  ft_putendl(path);
	i = -1;
	while (path[++i])
	{
		if (path[i] == '/')
			continue ;
		j = i;
/*		      ft_putstr("i = ");
		      ft_putnbr(i);
		      ft_putstr(" || j = ");
		      ft_putnbr(j);
		      ft_putstr("\n");
*/		if (path[j] && path[j++] != '.')
		{
			while (path[j] && path[j] != '/')
				++j;
/*			          ft_putstr("directory i = ");
			          ft_putnbr(i);
			          ft_putstr(" || j = ");
			          ft_putnbr(j);
			          ft_putstr("\n");
			          ft_putendl(ft_strsub(path, i, j - i));
		*/	tmp = current_path;
//			ft_putendl(current_path);
//			ft_putendl("second");
			current_path = ft_strjoin(tmp, ft_strsub(path, i - 1, j - i + 1));
			free(tmp);
		}
		else
		{
			while (path[j] == '.')
			{
//				ft_putstr("start\n");
				if (j++ != i)
				{
//					ft_putstr("pos = ");
					if (current_path)
						ft_putnbr(ftp_find_last_directory(current_path));
//					ft_putendl("");
					if (current_path && (pos = ftp_find_last_directory(current_path)))
						current_path = ft_strsub(current_path, 0, ft_strlen(current_path) - pos);
					else
					{
						free(current_path);
						current_path = NULL;
					}
				}
/*				ft_putstr("after . = ");
				if (current_path)
					ft_putstr(current_path);
				ft_putstr("\n");
*/			}
		}
		i = j;
	}
	tmp = current_path;
/*	ft_putstr("what = ");
	if (current_path)
		ft_putendl(current_path);
	ft_putendl("third");
*/	current_path = ft_strjoin(old_path, tmp);
	if (tmp)
		free(tmp);
	ft_putendl(current_path);
	return (current_path);
}
