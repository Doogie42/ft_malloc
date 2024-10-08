#include "malloc.h"

extern struct heap g_heap;

static t_zone *find_good_zone(void *addr, t_zone *first_zone) {
    t_zone *current = first_zone;
    if (!current) {
        return NULL;
    }
    while (current) {
        if ((char *)addr > (char *)current &&
            (char *)addr < (char *)current->end)
            return current;
        current = current->next;
    }
    return NULL;
}

t_zone *find_zone_ptr(void *addr) {
    t_zone *good_zone = find_good_zone(addr, g_heap.tiny_zone);
    if (good_zone == NULL) {
        good_zone = find_good_zone(addr, g_heap.small_zone);
    }
    return good_zone;
}
