#include "malloc.h"

static void *ft_memset(void *s, int c, size_t n) {
    size_t i;
    unsigned char *buf;

    if (!s)
        return (s);
    buf = s;
    i = 0;
    while (i < n) {
        *(buf + i) = c;
        i++;
    }
    return (s);
}

void *calloc(size_t nmemb, size_t size) {
	if (nmemb > size / SIZE_MAX)
		return NULL;
    void *ptr = malloc(nmemb * size);
	if (!ptr)
		return ptr;
    ft_memset(ptr, 0, nmemb * size);
	return ptr;
}