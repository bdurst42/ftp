/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/05 18:24:22 by bdurst            #+#    #+#             */
/*   Updated: 2017/06/05 18:28:04 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftp.h"

char		*ft_strjj(char *s1, char *s2, char *s3)
{
	char	*to_free;
	char	*rslt;

	to_free = ft_strjoin(s1, s2);
	rslt = ft_strjoin(to_free, s3);
	free(to_free);
	return (rslt);
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
