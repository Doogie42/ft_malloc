#include "malloc.h"

extern struct heap g_heap;

static void *search_ptr_in_zone_no_skip(void *addr, t_chunk *start, void *end) {
    t_chunk *tmp = start;
    void *addr_stop = end == NULL ? (void *)__UINT64_MAX__ : end;
    while (tmp && (void *)tmp < addr_stop) {
        if (tmp == addr)
            return tmp;
        tmp = tmp->next;
    }
    return NULL;
}
static void *search_ptr_in_zone(void *addr, t_chunk *start, void *end) {
    t_chunk *tmp = start;
    void *addr_stop = end == NULL ? (void *)__UINT64_MAX__ : end;
    while (tmp && (void *)tmp < addr_stop) {
        if (SKIP_HEADER_CHUNK(tmp) == addr)
            return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

static void *find_good_zone(void *addr, t_zone *first_zone) {
    if (!first_zone)
        return NULL;

    while (first_zone) {
        if (addr > (void *)first_zone && addr < (void *)first_zone->end)
            return first_zone;
        first_zone = first_zone->next;
    }
    return NULL;
}

void *find_zone_ptr(void *addr) {
    t_zone *good_zone = find_good_zone(addr, g_heap.tiny_zone);
    if (good_zone == NULL) {
        good_zone = find_good_zone(addr, g_heap.small_zone);
    }
    return good_zone;
}

void *search_ptr(void *addr, t_zone *good_zone, bool skip) {
    if (good_zone) {
        return skip == true ? search_ptr_in_zone(addr, SKIP_HEADER_ZONE(good_zone), good_zone->end) : search_ptr_in_zone_no_skip(addr, SKIP_HEADER_ZONE(good_zone), good_zone->end);
    } else if (g_heap.big_chunk) {
        return search_ptr_in_zone(addr, g_heap.big_chunk, NULL);
    }
    return NULL;
}

void *join_chunk(t_chunk *chunk) {
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
        if (chunk_zone == g_heap.tiny_zone ) {
            g_heap.tiny_zone = g_heap.tiny_zone->next;
            if (g_heap.tiny_zone)
                g_heap.tiny_zone->prev = NULL;

       		 munmap(chunk_zone, chunk_zone->end - (void *)chunk_zone);
				
            return ;

        } else if (chunk_zone == g_heap.small_zone) {
            g_heap.small_zone = g_heap.small_zone->next;
            if (g_heap.small_zone)
                g_heap.small_zone->prev = NULL;

       		 munmap(chunk_zone, chunk_zone->end - (void *)chunk_zone);

            return ;
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
    t_chunk *to_free = search_ptr(addr, chunk_zone, true);
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
    to_free = join_chunk(to_free);
    remove_empty_zone(to_free, chunk_zone);
}