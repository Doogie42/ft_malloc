#include "ft_printf.h"

int print_single_char(va_list *current_arg) {
    char c;

    c = va_arg(*current_arg, int);
    write(1, &c, 1);
    return (1);
}

int print_string(va_list *current_arg) {
    char *s;
    int size;

    s = va_arg(*current_arg, char *);
    if (s == NULL) {
        write(1, "(null)", 6);
        return (6);
    }
    size = 0;
    while (s && s[size]) size++;
    write(1, s, size);
    return (size);
}

int print_unsigned(va_list *current_arg) {
    uint64_t n;
    int len_number;
    char buffer[64] = {0};

    n = (unsigned int)va_arg(*current_arg, unsigned int);
    u_ft_itoa(n, buffer);
    len_number = ft_strlen(buffer);
    if (buffer[0] == '-')
        write(1, &buffer[1], len_number - 1);
    else
        write(1, buffer, len_number);
    return (len_number);
}

int print_char(char c) {
    write(1, &c, 1);
    return (1);
}

int print_non_special_char(const char *s, int *index) {
    int n_char_printed;
    int n_try_char_print;

    n_try_char_print = 0;
    n_char_printed = 0;
    while (s[*index] && s[*index] != '%') {
        n_try_char_print = 0;
        n_try_char_print = write(1, &s[*index], 1);
        if (n_try_char_print == -1) return (-1);
        n_char_printed += n_try_char_print;
        (*index)++;
    }
    return (n_char_printed);
}
