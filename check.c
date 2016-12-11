char                **ftp_get_args(char **args, char opt, char *path)
{
	int i;

	i = -1;
	while (args[++i])
	{
		if (i && (!opt || args[i][0] != '-'))
			args[i] = ftp_check_path(path, ft_strtrim(args[i]));
		else
			args[i] = ft_strtrim(args[i]);
	}
	return (args);
}

char            *ftp_check_path(char *old_path, char *path)
{
	int         i;
	int         j;
	int         pos;
	char        *current_path;

	ft_putstr("getcwd = ");
	//  ft_putendl(getcwd(NULL, 0));
	//  ft_putstr("old_path = ");
	//  ft_putendl(old_path);
	//  ft_putstr("ft_strstr = ");
	//  ft_putendl(ft_strstr(getcwd(NULL, 0), old_path));
	current_path = ft_strstr(getcwd(NULL, 0), old_path) + ft_strlen(old_path);
	//  ft_putstr("current_path = ");
	//  ft_putendl(current_path);
	//  if (!ft_strcmp(current_path, ""))
	//      current_path = "/";
	//  ft_putstr("path = ");
	//  ft_putendl(path);
	if (path && path[0] != '/')
		path = ft_strjoin("/", path);
	//  ft_putstr("path = ");
	//  ft_putendl(path);
	i = -1;
	while (path[++i])
	{
		if (path[i] == '/')
			continue ;
		j = i;
		//      ft_putstr("i = ");
		//      ft_putnbr(i);
		//      ft_putstr(" || j = ");
		//      ft_putnbr(j);
		//      ft_putstr("\n");
		if (path[j] && path[j++] != '.')
		{
			while (path[j] && path[j] != '/')
				++j;
			//          ft_putstr("directory i = ");
			//          ft_putnbr(i);
			//          ft_putstr(" || j = ");
			//          ft_putnbr(j);
			//          ft_putstr("\n");
			//          ft_putendl(ft_strsub(path, i, j - i));
			current_path = ft_strjoin(current_path, ft_strsub(path, i - 1, j - i + 1));
		}
		else
		{
			while (path[j] == '.')
				if (j++ != i)
					if ((pos = ftp_find_last_directory(current_path)))
						current_path = ft_strsub(current_path, 0, pos);
		}
		i = j;
	}
	current_path = ft_strjoin(old_path, current_path);
	//  ft_putendl(current_path);
	return (current_path);
}
