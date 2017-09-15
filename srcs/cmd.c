/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/06 15:58:40 by bdurst            #+#    #+#             */
/*   Updated: 2017/06/05 23:41:55 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

static void	ftp_cd(char *path, int c_sock)
{
	if ((chdir(path)) == -1)
		ftp_send_package("ERROR: cd failure", c_sock, 0, -1);
	else
		ftp_send_package("SUCCES: cd", c_sock, 0, -1);
	free(path);
}

static void	ftp_ls(char **args, int c_sock)
{
	pid_t	father;
	int		fd;
	int		stat_loc;

	if ((fd = open(FILE_BUFFER, O_RDWR | O_CREAT | O_TRUNC, 0777)) == -1)
		return ;
	if ((father = fork()) == 0)
	{
		if (dup2(fd, 1) < 0 || dup2(fd, 2) < 0)
			ftp_send_package("ERROR: dup2 failure", c_sock, 0, -1);
		else if (execv("/bin/ls", args) == -1)
		{
			ftp_send_package("ERROR: execv failure", c_sock, 0, -1);
			return ;
		}
	}
	else
	{
		wait4(0, &stat_loc, 0, 0);
		close(fd);
		ftp_send_package("ls", c_sock, 0, -1);
		ftp_send_file(FILE_BUFFER, c_sock, 0);
	}
	ft_free_tab(args);
}

static void	ftp_manage_cmd_dir(char *cmd, int c_sock, char *path, char del)
{
	int		i;
	char	**args;
	char	*msg;

	args = ft_strsplit(cmd, ' ');
	msg = NULL;
	i = 0;
	while (args[++i])
		msg = call_dir_function(del, path, args[i], msg);
	if (!msg)
	{
		if (del)
			msg = ft_strdup("SUCCES: rmdir");
		else
			msg = ft_strdup("SUCCES: mkdir");
	}
	else
	{
		(del) ? ftp_free_strjoin("ERROR: Can't delete ", &msg, 1) : \
		 ftp_free_strjoin("ERROR: Can't create ", &msg, 1);
	}
	ft_free_tab(args);
	free(msg);
	ftp_send_package(msg, c_sock, 0, -1);
}

static char	ftp_other_cmds(char *cmd, int c_sock, char *path)
{
	char	*c_p;

	if (!ft_strcmp(cmd, "pwd"))
	{
		c_p = getcwd(NULL, 0);
		ftp_send_package(c_p, c_sock, 0, -1);
		free(c_p);
	}
	else if (!ft_strncmp(cmd, "cd ", 3))
		ftp_cd(ftp_check_path(path, ft_strtrim(cmd + 3)), c_sock);
	else if (!ft_strcmp(cmd, "ls") || !ft_strncmp(cmd, "ls ", 3))
		ftp_ls(ftp_list_to_tabstr(ftp_get_args(ftp_tabstr_to_list(
		ft_strsplit(cmd, ' ')), 1, path)), c_sock);
	else if (!ft_strncmp(cmd, "mkdir ", 6))
		ftp_manage_cmd_dir(cmd, c_sock, path, 0);
	else if (!ft_strncmp(cmd, "rmdir ", 6))
		ftp_manage_cmd_dir(cmd, c_sock, path, 1);
	else
		ftp_send_package("Unknow command !", c_sock, 0, -1);
	return (1);
}

char		ftp_is_cmd(char *cmd, int c_sock, char *path)
{
	t_list	*list;
	t_tools	t;

	if (!cmd || cmd[0] == '\0' || !ft_strcmp(cmd, "quit"))
	{
		ftp_send_package("quit", c_sock, 0, -1);
		return (close(c_sock));
	}
	else if (!ft_strncmp(cmd, "get ", 4) && (list = LIST(path)))
	{
		t.sock = c_sock;
		t.flag = 1;
		t.cmd = cmd;
		ftp_manage_send_cmd(list->next, t, NULL);
		ft_clear_list(&list, (void*)&ftp_clear_list);
	}
	else if (!ft_strncmp(cmd, "put ", 4) && (list = LIST(NULL)))
	{
		ftp_manage_get_cmd(list->next, c_sock, 0);
		ftp_send_package("", c_sock, 0, -1);
		ft_clear_list(&list, (void*)&ftp_clear_list);
	}
	else
		return (ftp_other_cmds(cmd, c_sock, path));
	return (1);
}
