/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_map.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdurst <bdurst@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/12/17 14:33:54 by bdurst            #+#    #+#             */
/*   Updated: 2015/01/29 20:58:21 by bdurst           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

void	fdf_get_map(t_env *e)
{
	int		y;
	int		x;

	if ((e->map = (double **)malloc(sizeof(double *) * e->nrow)) == 0)
		ft_perror("malloc1 failed :", 0);
	y = 0;
	if (!e->tab || !(*e->tab) || !(**e->tab))
		exit(0);
	e->z_max = ft_atoi(e->tab[y][0]);
	e->z_min = ft_atoi(e->tab[y][0]);
	while (e->tab[y])
	{
		if ((e->map[y] = (double *)malloc(sizeof(double) * e->ncol)) == 0)
			ft_perror("malloc2 failed :", 0);
		x = 0;
		while (e->tab[y][x])
		{
			e->map[y][x] = ft_atoi(e->tab[y][x]);
			e->z_max = (e->z_max < e->map[y][x]) ? e->map[y][x] : e->z_max;
			if (e->z_min > e->map[y][x])
				e->z_min = e->map[y][x];
			++x;
		}
		++y;
	}
}
