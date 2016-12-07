#include "lemin.h"

t_list	*lm_malloc_lst(void)
{
	t_list *new;

	if (!(new = (t_list*)malloc(sizeof(t_list))))
		ft_exiterror("ERROR", -1);
	ft_bzero(&new, sizeof(t_list*));
	return (new);
}