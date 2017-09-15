#include "ftp.h"

static void	ftp_fill_list(t_list **list, char *end_path, char *join)
{
	t_arg			*arg;
	
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

void	    ftp_manage_stars(char *path, t_list **list, char *end_path)
{
	DIR				*dir;
	struct dirent	*ent;
	char			*dir_path;
	char			*join;

	dir_path = ft_strsub(path, 0, ftp_find_last_directory(path));
	if ((dir = ftp_opendir(dir_path, -2)))
	{
		while ((ent = readdir(dir)) != NULL)
		{
			join = ft_strjj(dir_path, "/", ent->d_name);
			if (ent->d_name[0] != '.' && (ftp_is_dir(join) || !end_path)
			&& nmatch(ent->d_name, path + ftp_find_last_directory(path) + 1))
				ftp_fill_list(list, end_path, join);
		}
		closedir(dir);
	}
	free(dir_path);
	free(path);
}
