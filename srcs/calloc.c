#include "malloc.h"

extern pthread_mutex_t g_mutex;

void *_calloc(size_t nmemb, size_t size) {
    void *ptr = internal_malloc(nmemb * size);
    memset(ptr, 0, nmemb * size);
    return ptr;
}

void *calloc(size_t nmemb, size_t size) {
    pthread_mutex_lock(&g_mutex);
    void *ptr = _calloc(nmemb, size);
    pthread_mutex_unlock(&g_mutex);
    return ptr;
}
