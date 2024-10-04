#include "malloc.h"

extern struct heap g_heap;

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

void *search_ptr(void *addr, t_zone *good_zone) {
    if (good_zone) {
        return search_ptr_in_zone(addr, SKIP_HEADER_ZONE(good_zone), good_zone->end);
    } else if (g_heap.big_chunk) {
        return search_ptr_in_zone(addr, g_heap.big_chunk, NULL);
    }
    return NULL;
}