/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_arg.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggay <ggay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/04 13:07:18 by ggay              #+#    #+#             */
/*   Updated: 2023/11/13 14:41:33 by ggay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	is_special_space(char c)
{
	if (c == '\t' || c == '\r' || c == '\v' || c == '\f' || c == '\n')
		return (1);
	return (0);
}

void	skip_space(const char *s, int *index)
{
	while (s[*index] && s[*index] == ' ')
		(*index)++;
}

int	get_conversion_type(const char *s, int *index)
{
	if (s[*index] && s[*index] == '%')
	{
		(*index)++;
		skip_space(s, index);
		if (is_special_space(s[*index]))
			return (CONV_IS_SPECIAL_SPACE);
		return (s[*index]);
	}
	return (0);
}

int	print_known_conversion_type(const char *s, int *index,
								va_list *current_arg, int conversion_type)
{
	if (conversion_type == 'c')
		return (print_single_char(current_arg));
	else if (conversion_type == 's')
		return (print_string(current_arg));
	else if (conversion_type == 'd' || conversion_type == 'i')
		return (print_decimal(current_arg));
	else if (conversion_type == 'x' || conversion_type == 'X')
		return (print_hexa(current_arg, conversion_type));
	else if (conversion_type == 'p')
		return (print_pointer(current_arg));
	else if (conversion_type == 'u')
		return (print_unsigned(current_arg));
	else if (conversion_type == '%')
		return (print_char('%'));
	else if (conversion_type == CONV_IS_SPECIAL_SPACE)
		return (print_char('%') + print_non_special_char(s, index));
	else
		return (-1);
	return (1);
}

int	print_arg(const char *s, int *index, va_list *current_arg)
{
	int	conversion_type;
	int	nb_char_written;

	if (s[*index] && s[*index] == '%' && s[*index + 1])
	{
		conversion_type = get_conversion_type(s, index);
		nb_char_written = print_known_conversion_type
			(s, index, current_arg, conversion_type);
		if (nb_char_written != -1)
		{
			(*index)++;
			return (nb_char_written);
		}
		else
		{
			write (1, &s[*index - 1], 1);
			write (1, &s[*index], 1);
			(*index)++;
			return (2);
		}
	}
	return (0);
}
