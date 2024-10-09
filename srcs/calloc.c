#include "malloc.h"

extern pthread_mutex_t g_mutex;

static void *ft_memset(void *s, int c, size_t n) {
    size_t i;
    unsigned char *buf;

    if (!s) return (s);
    buf = s;
    i = 0;
    while (i < n) {
        *(buf + i) = c;
        i++;
    }
    return (s);
}

void *_calloc(size_t nmemb, size_t size) {
    if (size > 0 && nmemb > __INT_MAX__ / size) return NULL;

    void *ptr = internal_malloc(nmemb * size);
    if (!ptr) return NULL;
    ft_memset(ptr, 0, nmemb * size);
    return ptr;
}

void *calloc(size_t nmemb, size_t size) {
    pthread_mutex_lock(&g_mutex);
    void *ptr = _calloc(nmemb, size);
    pthread_mutex_unlock(&g_mutex);
    return ptr;
}
