#include "malloc.h"

extern struct heap g_heap;
extern pthread_mutex_t g_mutex;

static t_chunk *defragment(t_chunk *chunk) {
    t_chunk *next = chunk->next;
    t_chunk *prev = chunk->prev;

    if (CHUNK_FREE(next)) {
        chunk->size += CHUNK_SIZE(next->size) + sizeof(t_chunk);
        chunk->next = next->next;
        next->next->prev = chunk;
        SET_CHUNK_FREE(chunk);
    }
    if (CHUNK_FREE(prev)) {
        prev->size += CHUNK_SIZE(chunk->size) + sizeof(t_chunk);
        prev->next = chunk->next;
        chunk->next->prev = prev;
        SET_CHUNK_FREE(prev);
        return prev;
    }
    return chunk;
}

static void free_big(t_chunk *chunk) {
    if (chunk == g_heap.big_chunk) {
        g_heap.big_chunk = chunk->next;
        if (g_heap.big_chunk && g_heap.big_chunk->next) {
            g_heap.big_chunk->next->prev = g_heap.big_chunk;
        }
    } else {
        chunk->prev->next = chunk->next;
        if (chunk->next) {
            chunk->next->prev = chunk->prev;
        }
    }
    SET_CHUNK_FREE(chunk);
    munmap(chunk, chunk->size + sizeof(t_chunk));
    return;
}

static void remove_empty_zone(t_chunk *chunk) {
    size_t true_size = chunk->size + sizeof(t_chunk);
    if (true_size + 2 * GUARD_CHUNK_SIZE !=
            g_heap.option.small_size_zone - sizeof(t_zone) &&
        true_size + 2 * GUARD_CHUNK_SIZE !=
            g_heap.option.tiny_size_zone - sizeof(t_zone))
        return;

    t_zone *empty_zone = find_zone_ptr(chunk);

    // if we only have one zone we keep it allocated
    // if (!empty_zone->next && !empty_zone->prev) return;

    if (empty_zone->next) {
        empty_zone->next->prev = empty_zone->prev;
    }
    if (empty_zone->prev) {
        empty_zone->prev->next = empty_zone->next;
    }
    void *addr = (char *)empty_zone - sizeof(t_zone);
    size_t size =
        (char *)empty_zone->end - (char *)empty_zone + sizeof(t_chunk);
    munmap(addr, size);
}

void internal_free(void *addr) {
    if (!addr) return;

#ifndef NO_SAFE_FREE
    t_chunk *chunk = find_ptr(addr);
#else
	t_chunk *chunk = MEM_TO_CHUNK(addr);
	if (!chunk % ALIGN_SIZE){
        LOG("free(): invalid pointer\n");
	}
#endif

	if (!chunk) {
        LOG("free(): invalid pointer\n");
        return;
    }
	
    if (CHUNK_FREE(chunk)) {
        LOG("free(): double free detected\n");
        return;
    }
    if (CHUNK_SIZE(chunk->size) > g_heap.option.small_size_chunk) {
        free_big(chunk);
        return;
    }
    chunk = defragment(chunk);
    SET_CHUNK_FREE(chunk);
    remove_empty_zone(chunk);
}

void free(void *addr) {
    pthread_mutex_lock(&g_mutex);
    internal_free(addr);
    pthread_mutex_unlock(&g_mutex);
}

void delete_all_zone(t_zone *first_zone) {
    t_zone *current = first_zone;
    while (current) {
        t_zone *next = current->next;
        munmap(current, (char *)current->end - (char *)current);
        current = next;
    }
}

void __attribute__((destructor)) clean_up(void){
    delete_all_zone(g_heap.tiny_zone);
    delete_all_zone(g_heap.small_zone);
    t_chunk *current = g_heap.big_chunk;
    while (current) {
        t_chunk *next = current->next;
        munmap(current, current->size);
        current = next;
    }

}
