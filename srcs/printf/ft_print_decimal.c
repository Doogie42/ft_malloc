#include "ft_printf.h"

int print_decimal(va_list *current_arg) {
    int n;
    int len_s;
    char buffer[64] = {0};

    n = va_arg(*current_arg, int);
    ft_itoa(n, buffer);
    len_s = ft_strlen(buffer);
    write(1, buffer, len_s);
    return (len_s);
}
