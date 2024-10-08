#include "ft_printf.h"

int print_custom_nbr_zero(char s[], int len_number, int n_zero) {
    int i;

    i = 0;
    while (i < (len_number - 1) && s[i] == '0') i++;
    if (i - n_zero >= 0) i = i - n_zero;
    write(1, &s[i], len_number - i - 1);
    return (len_number - i - 1);
}

int get_len_hexa(uint64_t n) {
    int len;

    len = 0;
    while (n > 0) {
        n = n / 16;
        len++;
    }
    return (len);
}

void int_to_hex(uint64_t n, char s[], int len_number) {
    int len;
    int i;

    i = 0;
    len = len_number - 2;
    while (i < len_number - 1) {
        s[i] = '0';
        i++;
    }
    s[len_number - 1] = '\0';
    while (n > 0) {
        s[len] = n % 16 + '0';
        n = n / 16;
        len--;
    }
}

int print_hexa(va_list *current_arg, int maj) {
    int t;
    char number_hexa[9];
    int i;
    uint64_t n;

    t = va_arg(*current_arg, int);
    n = t;
    if (n == 0) {
        write(1, "0", 1);
        return (1);
    }
    int_to_hex(n, number_hexa, 9);
    i = 0;
    while (number_hexa[i]) {
        if (number_hexa[i] > '9' && maj == 'x')
            number_hexa[i] += 39;
        else if (number_hexa[i] > '9' && maj == 'X')
            number_hexa[i] += 7;
        i++;
    }
    return (print_custom_nbr_zero(number_hexa, 9, 0));
}
