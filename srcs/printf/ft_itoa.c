#include "ft_printf.h"

static size_t get_size_string(int n) {
    size_t size;

    size = 0;
    if (n <= 0) size++;
    while (n) {
        size++;
        n = n / 10;
    }
    return (size);
}

static void fill_string(char *s, int n, size_t len_s) {
    int sign;

    sign = 1;
    if (n < 0) {
        s[0] = '-';
        sign = -1;
    }
    while (n) {
        s[len_s] = (n % 10) * sign + '0';
        n = n / 10;
        len_s--;
    }
}

void ft_itoa(int n, char buffer[64]) {
    size_t size_string;

    size_string = get_size_string(n);
    if (size_string == 0) return;

    if (n == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    fill_string(buffer, n, size_string - 1);
    buffer[size_string] = '\0';
    return;
}
