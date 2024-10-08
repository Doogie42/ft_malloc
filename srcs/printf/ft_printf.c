#include "ft_printf.h"

int ft_printf(const char *s, ...) {
    int i;
    va_list current_arg;
    int nb_char_written;

    nb_char_written = 0;
    i = 0;
    va_start(current_arg, s);
    while (s[i]) {
        if (s[i] == '%' && s[i + 1] == '\0') break;
        nb_char_written += print_non_special_char(s, &i);
        nb_char_written += print_arg(s, &i, &current_arg);
    }
    va_end(current_arg);
    return (nb_char_written);
}
