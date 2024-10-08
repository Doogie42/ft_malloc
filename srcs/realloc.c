#include "malloc.h"

extern struct heap g_heap;
extern pthread_mutex_t g_mutex;

static void *full_dealloc(t_chunk *chunk, size_t size) {
    void *new_ptr = internal_malloc(size);
    size_t minimum_copy = chunk->size < size ? chunk->size : size;
    memcpy(new_ptr, SKIP_HEADER_CHUNK(chunk), minimum_copy);
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
    t_chunk *chunk = MEM_TO_CHUNK(ptr);
    return full_dealloc(chunk, size);
    if (chunk->size > g_heap.option.small_size_chunk) {
        return full_dealloc(chunk, size);
    }
    if (size <= CHUNK_SIZE(chunk->size)) {
        return SKIP_HEADER_CHUNK(chunk);
    }
    if (CHUNK_FREE(chunk->next) && size > CHUNK_SIZE(chunk->size) &&
        chunk->next->size > size - CHUNK_SIZE(chunk->size)) {
        size_t added_size = size - chunk->size;
        t_chunk *extra_chunk = split_free_chunk(chunk->next, added_size);
        chunk->next = extra_chunk->next;
        extra_chunk->next->prev = chunk;
        chunk->size = (char *)chunk->next - (char *)chunk - sizeof(t_chunk);
        SET_CHUNK_USED(chunk);
        return SKIP_HEADER_CHUNK(chunk);
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
