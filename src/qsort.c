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
DESCRIPTION:
	Quicksorts the environment variables in alphabetical order.
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
DESCRIPTION:
	Partitions the environment variables into two groups:
		- less than or equal to the pivot
		- greater than the pivot
	Pivot used is the last element in the array (high).
	Returns the index of the pivot element.

[ <= pivot (i) | > pivot (j) | unknown | pivot (high)]
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

/*
DESCRIPTION:
	Swaps the environment variables at the given indices.
*/

static void	swap_env(t_env **a, t_env **b)
{
	t_env	*tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}
