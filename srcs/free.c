#include "malloc.h"

extern struct heap g_heap;

void *defragment_chunk(t_chunk *chunk) {
    t_chunk *next = chunk->next;
    t_chunk *prev = chunk->prev;
    if (next && CHUNK_FREE(next)) {
        chunk->size += CHUNK_SIZE(next->size) + sizeof(t_chunk);
        next->next->prev = chunk;
        chunk->next = next->next;
    }
    if (prev && CHUNK_FREE(prev)) {
        prev->size += CHUNK_SIZE(chunk->size) + sizeof(t_chunk);
        prev->next = chunk->next;
        chunk->next->prev = prev;
        return prev;
    }
    return chunk;
}

void *unlink_zone(t_zone *zone) {
    t_chunk *begin_guard_chunk = SKIP_HEADER_ZONE(zone);
    t_chunk *end_guard_chunk = (void *)zone->end - GUARD_CHUNK_SIZE;
    t_zone *previous_zone = zone->prev;
    t_zone *next_zone = zone->next;
    if (previous_zone) {
        t_chunk *end_guard_chunk_prev = (void *)previous_zone->end - GUARD_CHUNK_SIZE;
        end_guard_chunk_prev->next = end_guard_chunk->next;
    }
    if (next_zone) {
        t_chunk *begin_guard_chunk_next = SKIP_HEADER_ZONE(next_zone);
        begin_guard_chunk_next->prev = begin_guard_chunk->prev;
    }
    return zone;
}

void remove_empty_zone(t_chunk *to_free, t_zone *chunk_zone) {
    if ((void *)to_free->prev == SKIP_HEADER_ZONE(chunk_zone) &&
        (void *)to_free->next + GUARD_CHUNK_SIZE == chunk_zone->end) {
        chunk_zone = unlink_zone(chunk_zone);
        t_zone *previous_zone = chunk_zone->prev;
        if (chunk_zone == g_heap.tiny_zone) {
            g_heap.tiny_zone = g_heap.tiny_zone->next;
            if (g_heap.tiny_zone)
                g_heap.tiny_zone->prev = NULL;

            munmap(chunk_zone, chunk_zone->end - (void *)chunk_zone);

            return;

        } else if (chunk_zone == g_heap.small_zone) {
            g_heap.small_zone = g_heap.small_zone->next;
            if (g_heap.small_zone)
                g_heap.small_zone->prev = NULL;

            munmap(chunk_zone, chunk_zone->end - (void *)chunk_zone);

            return;
        }
        if (previous_zone)
            previous_zone->next = chunk_zone->next;
        if (chunk_zone->next)
            chunk_zone->next->prev = previous_zone;
        munmap(chunk_zone, chunk_zone->end - (void *)chunk_zone);
    }
}

void free(void *addr) {
    if (addr == NULL)
        return;
    t_zone *chunk_zone = find_zone_ptr(addr);
    if (!chunk_zone) {
        ft_printf("free (): invalid pointer %p\n", addr);
        return;
    }
    t_chunk *to_free = search_ptr(addr, chunk_zone);
    if (to_free == NULL) {
        ft_printf("free (): invalid pointer %p\n", addr);
        // abort();
        return;
    }
    if (CHUNK_FREE(to_free)) {
        ft_printf("free(): Double free detected\n");
        // abort();
        return;
    }
    SET_CHUNK_FREE(to_free);
    to_free = defragment_chunk(to_free);
    remove_empty_zone(to_free, chunk_zone);
}