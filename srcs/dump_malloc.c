#include "malloc.h"

extern struct heap g_heap;

static void dump_zone(bool show_data, bool show_header, t_zone *start_zone) {
    t_chunk *current = (t_chunk *)((void *)start_zone + sizeof(t_zone));
    t_zone *zone = start_zone;
    while (zone) {
        ft_printf("zone %p end %p size %d next %p\n", zone, zone->end,zone->end - (void *) zone, zone->next);
        zone = zone->next;
    }

    t_chunk *data = 0;
	if (!show_data && !show_header)
		return;
    while (current) {
        // if (current->size == 0)
        // 	break;

        if (show_header)
            data = current;
        else
            data = SKIP_HEADER_CHUNK(current);

        ft_printf("%p (%p)", current, SKIP_HEADER_CHUNK(current));
        if (current->next != NULL)
            ft_printf("- %p ", current->next);
        else
            ft_printf("- N 0xxxxxxxxxxx ");

        ft_printf(" : %d bytes", CHUNK_SIZE(current->size));
        ft_printf(" free %s", CHUNK_FREE(current) ? "true" : "false");
        if (show_data && !CHUNK_FREE(current)) {
            ft_printf("\n DUMP:");
            size_t size = CHUNK_SIZE(current->size);
            if (show_header)
                size += sizeof(t_chunk);

            for (size_t i = 0; i < size; i++) {
                char *d = (char *)data;
                if (i % 8 == 0 && i != 0)
                    ft_printf("|");
                ft_printf("%x ", d[i]);
            }
        }
        ft_printf("\n");
        current = current->next;
    }
}

void dump_malloc(bool show_data, bool show_header) {
    if (g_heap.tiny_zone) {
        ft_printf("TINY ZONE\n");
        dump_zone(show_data, show_header, g_heap.tiny_zone);
    }
    if (g_heap.small_zone) {
        ft_printf("SMALL ZONE\n");
        dump_zone(show_data, show_header, g_heap.small_zone);
    }
}
