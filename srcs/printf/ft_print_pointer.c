#include "ft_printf.h"

void add_hexa_prefix(char *hexa_number) {
    size_t i;

    i = 0;
    while (hexa_number[i] && hexa_number[i] == '0') i++;
    if (i < 2) return;
    hexa_number[i - 1] = 'x';
    hexa_number[i - 2] = '0';
}

int print_pointer(va_list *current_arg) {
    void *t;
    char number_hexa[20];
    int i;
    uint64_t n;

    t = va_arg(*current_arg, void *);
    n = (uint64_t)t;
    if (n == 0) {
        write(1, "(nil)", 5);
        return (5);
    }
    int_to_hex(n, number_hexa, 20);
    i = 0;
    while (number_hexa[i]) {
        if (number_hexa[i] > '9') number_hexa[i] += 39;
        i++;
    }
    add_hexa_prefix(number_hexa);
    return (print_custom_nbr_zero(number_hexa, 20, 1));
}
