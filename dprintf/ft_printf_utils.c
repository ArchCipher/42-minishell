/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 12:38:21 by kmurugan          #+#    #+#             */
/*   Updated: 2025/11/19 17:51:03 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_dprintf.h"

int	ft_atoi_positive(const char *str)
{
	int	num;

	num = 0;
	while (ft_isdigit(*str))
	{
		if (INT_MAX - num * 10 <= (*str - '0'))
			return (INT_MAX);
		num = (num * 10) + (*str - '0');
		str++;
	}
	return (num);
}
