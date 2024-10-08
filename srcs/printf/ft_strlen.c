#include "ft_printf.h"

int ft_strlen(const char *s) {
    int size;

    size = 0;
    while (s[size]) size++;
    return (size);
}
