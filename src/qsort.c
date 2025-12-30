/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   qsort.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:45:18 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:52:28 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	partition(t_env **arr, size_t low, size_t high);
static void	swap_env(t_env **a, t_env **b);

/*
nel = number of elements
width = size of each element
*/
void	ft_qsort_env(t_env **arr, size_t low, size_t high)
{
	size_t	pi;

	if (low < high)
	{
		pi = partition(arr, low, high);
		if (pi > 0)
			ft_qsort_env(arr, low, pi - 1);
		ft_qsort_env(arr, pi + 1, high);
	}
}

/*
[ <= pivot (i) | > pivot (j) | unknown | pivot ]
*/

static size_t	partition(t_env **arr, size_t low, size_t high)
{
	size_t	i;
	size_t	j;

	i = low;
	j = low;
	while (j < high)
	{
		if (ft_strcmp(arr[j]->key, arr[high]->key) <= 0)
		{
			swap_env(&arr[i], &arr[j]);
			i++;
		}
		j++;
	}
	swap_env(&arr[i], &arr[high]);
	return (i);
}

static void	swap_env(t_env **a, t_env **b)
{
	t_env	*tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}
