#include "malloc.h"

extern struct heap g_heap;
extern pthread_mutex_t g_mutex;

void *ft_memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d;
    const unsigned char *s;
    size_t i;

    if (!dest || !src) return (dest);
    d = dest;
    s = src;
    i = 0;
    while (i < n) {
        d[i] = s[i];
        i++;
    }
    return (dest);
}

static void *full_dealloc(t_chunk *chunk, size_t size) {
    void *new_ptr = internal_malloc(size);
    if (!new_ptr) return NULL;
    size_t minimum_copy = chunk->size < size ? chunk->size : size;
    ft_memcpy(new_ptr, SKIP_HEADER_CHUNK(chunk), minimum_copy);
    internal_free(SKIP_HEADER_ZONE(chunk));
    return new_ptr;
}

void *_realloc(void *ptr, size_t size) {
    if (!ptr) {
        return internal_malloc(size);
    }
    if (size == 0) {
        internal_free(ptr);
        return ptr;
    }
    size = align_mem(size);
#ifndef NO_SAFE_FREE
    t_chunk *chunk = find_ptr(ptr);
#else
	t_chunk *chunk = MEM_TO_CHUNK(ptr);
#endif
	if (!chunk){
		return NULL;
	}
	if (CHUNK_FREE(chunk))
		return NULL;
    if (chunk->size > g_heap.option.small_size_chunk ||
        size > g_heap.option.small_size_chunk) {
        return full_dealloc(chunk, size);
    }
    if (size <= CHUNK_SIZE(chunk->size)) {
        return SKIP_HEADER_CHUNK(chunk);
    }

    size_t added_size = size - CHUNK_SIZE(chunk->size);
    if (CHUNK_FREE(chunk->next) && CHUNK_SIZE(chunk->next->size) > added_size) {
        t_chunk *extra_chunk = split_free_chunk(chunk->next, added_size);
        chunk->next = extra_chunk->next;
        chunk->next->prev = chunk;
        chunk->size = (char *)chunk->next - (char *)chunk - sizeof(t_chunk);

        SET_CHUNK_USED(chunk);
        SET_CHUNK_USED(chunk->next);
        return ptr;
    } else {
        return full_dealloc(chunk, size);
    }
}

void *realloc(void *ptr, size_t size) {
    pthread_mutex_lock(&g_mutex);
    void *ret = _realloc(ptr, size);
    pthread_mutex_unlock(&g_mutex);
    return ret;
}
