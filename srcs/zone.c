#include "malloc.h"

extern struct heap g_heap;

t_zone_info get_zone_from_size(size_t size) {
    t_zone_info zone_info;
    if (size <= g_heap.option.tiny_size_chunk) {
        zone_info.size_chunk = g_heap.option.tiny_size_chunk;
        zone_info.zone = &g_heap.tiny_zone;
        zone_info.type = e_tiny_zone;
        zone_info.size_zone = g_heap.option.tiny_size_zone;
        zone_info.smallest_chunk_size = 64;
    } else if (size <= g_heap.option.small_size_chunk) {
        zone_info.size_chunk = g_heap.option.small_size_chunk;
        zone_info.zone = &g_heap.small_zone;
        zone_info.type = e_small_zone;
        zone_info.size_zone = g_heap.option.small_size_zone;
        zone_info.smallest_chunk_size = g_heap.option.tiny_size_chunk;
    } else {
        zone_info.type = e_big_zone;
    }
    return zone_info;
}

t_zone* init_zone(t_zone_info zone_info) {
    t_zone* new_zone = mmap(NULL, zone_info.size_zone, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANON, -1, 0);
    if (!new_zone) return NULL;
    new_zone->end = (char*)new_zone + zone_info.size_zone;
    new_zone->next = NULL;
    new_zone->prev = NULL;
    t_chunk* guard_chunk_begin = SKIP_HEADER_ZONE(new_zone);
    t_chunk* guard_chunk_end = GET_END_GUARD(new_zone);
    t_chunk* free_chunk = SKIP_BEGIN_GUARD_CHUNK(new_zone);
    guard_chunk_begin->size = GUARD_CHUNK_SIZE - sizeof(t_chunk);
    free_chunk->size = zone_info.size_zone - 2 * GUARD_CHUNK_SIZE -
                       sizeof(t_chunk) - sizeof(t_zone);
    guard_chunk_end->size = GUARD_CHUNK_SIZE - sizeof(t_chunk);

    guard_chunk_begin->next = free_chunk;
    free_chunk->next = guard_chunk_end;
    guard_chunk_end->next = NULL;

    guard_chunk_begin->prev = NULL;
    free_chunk->prev = guard_chunk_begin;
    guard_chunk_end->prev = free_chunk;

    SET_CHUNK_USED(guard_chunk_begin);
    SET_CHUNK_USED(guard_chunk_end);
    SET_CHUNK_FREE(free_chunk);
    return new_zone;
}

t_zone* link_zone(t_zone* first_zone, t_zone* new_zone) {
    t_zone* current_zone = first_zone;
    while (current_zone->next) {
        current_zone = current_zone->next;
    }
    current_zone->next = new_zone;

    t_chunk* guard_chunk_end = GET_END_GUARD(current_zone);
    t_chunk* guard_chunk_begin_new_zone = SKIP_HEADER_ZONE(new_zone);
    guard_chunk_end->next = guard_chunk_begin_new_zone;
    guard_chunk_begin_new_zone->prev = guard_chunk_end;
    return first_zone;
}
