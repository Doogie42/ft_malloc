#include "malloc.h"

extern struct heap g_heap;

static void dump_addr(bool show_data, bool show_header, t_chunk *current) {
    ft_printf("%p (%p)", current, SKIP_HEADER_CHUNK(current));
    if (current->next != NULL)
        ft_printf("- %p ", current->next);
    else
        ft_printf("- N 0xxxxxxxxxxx ");

    ft_printf(" : %d bytes", CHUNK_SIZE(current->size));
    ft_printf(" free %s", CHUNK_FREE(current) ? "true" : "false");
    ft_printf("\n");
    if (show_header) {
        ft_printf("HEADER : next %p | prev %p | size %d\n", current->next,
                  current->prev, current->size);
        ft_printf("DUMP H :");
        t_chunk *data = current;

        for (size_t i = 0; i < sizeof(t_chunk); i++) {
            char *d = (char *)data;
            if (i % 8 == 0 && i != 0) ft_printf("|");
            ft_printf("%x ", d[i]);
        }
    }
    ft_printf("\n");
    if (show_data) {
        t_chunk *data = SKIP_HEADER_CHUNK(current);

        size_t size = CHUNK_SIZE(current->size);
        if (CHUNK_FREE(current)) size = 64;
        ft_printf("DATA: ");
        if (size == 0) {
            ft_printf("!!CORRUPTED DATA!!: ");
            size = 64;
        }
        for (size_t i = 0; i < size; i++) {
            char *d = (char *)data;
            if (i % 8 == 0 && i != 0) ft_printf("|");
            ft_printf("%x ", d[i]);
        }
    }
    ft_printf("\n");
}

static void dump_chunk(bool show_data, bool show_header, t_chunk *current) {
    if (!show_data && !show_header) return;
    while (current) {
        dump_addr(show_data, show_header, current);
        current = current->next;
    }
}

static void dump_zone(bool show_data, bool show_header, t_zone *start_zone) {
    t_chunk *current = SKIP_HEADER_ZONE(start_zone);
    t_zone *zone = start_zone;
    while (zone) {
        ft_printf("zone %p end %p size %d next %p\n", zone, zone->end,
                  zone->end - (char *)zone, zone->next);
        zone = zone->next;
    }
    dump_chunk(show_data, show_header, current);
}

void show_alloc_mem_ex(bool show_data, bool show_header) {
    ft_printf("size of tchunk %d\n", sizeof(t_chunk));
    if (g_heap.tiny_zone) {
        ft_printf("TINY ZONE\n");
        dump_zone(show_data, show_header, g_heap.tiny_zone);
    }
    if (g_heap.small_zone) {
        ft_printf("SMALL ZONE\n");
        dump_zone(show_data, show_header, g_heap.small_zone);
    }
    if (g_heap.big_chunk) {
        ft_printf("BIG ZONE\n");
        dump_chunk(show_data, show_header, g_heap.big_chunk);
    }
}

void show_alloc_mem() {
    size_t allocated = 0;
    t_zone *zone = g_heap.tiny_zone;

    while (zone) {
        ft_printf("TINY: %p\n", zone);
        t_chunk *chunk = SKIP_HEADER_CHUNK(zone);
        while (chunk) {
            if (CHUNK_SIZE(chunk->size) != 0 && !CHUNK_FREE(chunk)) {
                ft_printf("%p - %p : %d bytes\n", SKIP_HEADER_CHUNK(chunk),
                          (char *)chunk + CHUNK_SIZE(chunk->size),
                          CHUNK_SIZE(chunk->size));
                allocated += CHUNK_SIZE(chunk->size);
            }
            chunk = chunk->next;
        }
        zone = zone->next;
    }
    zone = g_heap.small_zone;

    while (zone) {
        ft_printf("SMALL: %p\n", zone);
        t_chunk *chunk = SKIP_HEADER_CHUNK(zone);
        while (chunk) {
            if (CHUNK_SIZE(chunk->size) != 0 && !CHUNK_FREE(chunk)) {
                ft_printf("%p - %p : %d bytes\n", SKIP_HEADER_CHUNK(chunk),
                          (char *)chunk + CHUNK_SIZE(chunk->size),
                          CHUNK_SIZE(chunk->size));
                allocated += CHUNK_SIZE(chunk->size);
            }
            chunk = chunk->next;
        }
        zone = zone->next;
    }
    t_chunk *chunk = g_heap.big_chunk;
    while (chunk) {
        ft_printf("LARGE : %p\n", chunk);
        if (CHUNK_SIZE(chunk->size) != 0 && !CHUNK_FREE(chunk)) {
            ft_printf("%p - %p : %d bytes\n", SKIP_HEADER_CHUNK(chunk),
                      (char *)chunk + CHUNK_SIZE(chunk->size),
                      CHUNK_SIZE(chunk->size));
            allocated += CHUNK_SIZE(chunk->size);
        }
        chunk = chunk->next;
    }
    ft_printf("Total : %d bytes\n", allocated);
}
