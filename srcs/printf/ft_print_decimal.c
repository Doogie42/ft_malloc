/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_decimal.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggay <ggay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/04 13:28:39 by ggay              #+#    #+#             */
/*   Updated: 2023/11/15 15:03:13 by ggay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	print_decimal(va_list *current_arg)
{
	int		n;
	int		len_s;
	char	buffer[64] = {0};

	n = va_arg(*current_arg, int);
	ft_itoa(n, buffer);
	len_s = ft_strlen(buffer);
	write (1, buffer, len_s);
	return (len_s);
}
