#include <stdio.h>
#include <stdbool.h>

extern void *malloc(size_t size);
extern void show_alloc_mem(bool show_data, bool show_header);
extern void *calloc(size_t nmemb, size_t size);
extern void *realloc(void *ptr, size_t size);
extern void free(void *ptr);