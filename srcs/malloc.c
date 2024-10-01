#include "malloc.h"

typedef struct chunk t_chunk;

struct chunk {
    size_t size;
    t_chunk *next;
    t_chunk *prev;
};

typedef struct option {
    size_t tiny_size_zone;
    size_t tiny_size_chunk;
    size_t small_size_zone;
    size_t small_size_chunk;
} t_option;

typedef struct _zone {
    void *end;
    void *next;
} t_zone;

struct heap {
    t_option option;
    t_zone *tiny_zone;
    t_zone *small_zone;
};

struct option init_option() {
    struct option option = {
        .tiny_size_zone = sysconf(_SC_PAGESIZE) * 4,
        .tiny_size_chunk = option.tiny_size_zone / 256,
        .small_size_zone = sysconf(_SC_PAGESIZE) * 64,
        .small_size_chunk = option.small_size_zone / 128,
    };
    return option;
}

// struct option init_option() {
//     struct option option = {
//         .small_size_zone = sysconf(_SC_PAGESIZE) * 4,
//         .small_size_chunk = option.small_size_zone / 256,
//         .tiny_size_zone = sysconf(_SC_PAGESIZE) * 64,
//         .tiny_size_chunk = option.tiny_size_zone / 128,
//     };
//     return option;
// }

#define SKIP_HEADER_ZONE(zone) ((void *)zone + sizeof(t_zone))
#define SKIP_HEADER_CHUNK(chunk) ((void *)chunk + sizeof(t_chunk))

// Because our address is 16 bit aligned the size of each chunk is at least 16 bit
// -> we can use the last bits to store data
// -> rightmost bit will tell us if chunk is free or not -> no more bool :)
#define CHUNK_FREE(x) ((x->size & -x->size) - 1)
// We unset the last bit if set
#define CHUNK_SIZE(x) (size_t)(x & ~1)

#define SET_CHUNK_USED(x) x->size ^= 1
#define SET_CHUNK_FREE(x) x->size &= ~1

struct heap g_heap;

void *make_new_free_chunk(size_t size, t_chunk *prev, t_zone *new_zone);

t_zone *create_new_zone(size_t size) {
    t_zone *new_zone = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    new_zone->next = NULL;
    new_zone->end = (void *)new_zone + size;
    LOG("new zone created %p with size %d end is %p\n", new_zone, size, new_zone->end);

    t_zone *current = size == g_heap.option.tiny_size_zone ? g_heap.tiny_zone : g_heap.small_zone;
    if (!current) {
        current = new_zone;
        return current;
    }
    while (current->next) {
        current = current->next;
    }
    current->next = new_zone;

    return new_zone;
}

t_chunk *split_chunk(t_chunk *free_chunk, size_t size) {
    t_chunk *new_chunk = free_chunk;
    size_t new_free_size = free_chunk->size - size - sizeof(t_chunk);
    t_chunk *old_next = free_chunk->next;
    new_chunk->size = size;
    SET_CHUNK_USED(new_chunk);
    free_chunk = (void *)new_chunk + size + sizeof(t_chunk);
    new_chunk->next = free_chunk;
    (free_chunk)->prev = new_chunk;
    (free_chunk)->size = new_free_size;
    SET_CHUNK_FREE(free_chunk);
    free_chunk->next = old_next;

    return new_chunk;
}

void *make_new_free_chunk(size_t size, t_chunk *prev, t_zone *new_zone) {
    t_chunk *new_free_chunk = SKIP_HEADER_ZONE(new_zone);
    new_free_chunk->size = size - sizeof(t_zone) - sizeof(t_chunk);

    if (prev) {
        t_chunk *old_next = prev->next;
        new_free_chunk->prev = prev;
        prev->next = new_free_chunk;
        new_free_chunk->next = old_next;
    }
    return new_free_chunk;
}

void *alloc(size_t size, t_zone **zone) {
    t_chunk *start;
    if (!*zone) {
        size_t zone_size = size < g_heap.option.tiny_size_chunk ? g_heap.option.tiny_size_zone : g_heap.option.small_size_zone;
        *zone = create_new_zone(zone_size);
        start = make_new_free_chunk(zone_size, NULL, *zone);
    } else {
        start = SKIP_HEADER_ZONE(*zone);
    }

    bool found = false;
    t_chunk *prev = NULL;

    while (1) {
        if (CHUNK_FREE(start) && CHUNK_SIZE(start->size) >= sizeof(t_chunk) + size) {
            found = true;
            break;
        }
        if (!start->next)
            break;
        prev = start;
        start = start->next;
    }
    // Need a new zone
    if (!found) {
        size_t zone_size = size < g_heap.option.tiny_size_chunk ? g_heap.option.tiny_size_zone : g_heap.option.small_size_zone;
        t_zone *new_zone = create_new_zone(zone_size);
        start = make_new_free_chunk(zone_size, start, new_zone);
    }
    start = split_chunk(start, size);
    return SKIP_HEADER_CHUNK(start);
}

size_t align_mem(size_t size) {
    unsigned int memalign = 16;
    if (size < memalign)
        size = memalign;
    else if (size % memalign != 0)
        size = size + memalign - size % memalign;
    return size;
}

void *_malloc(size_t size) {
    size = align_mem(size);
    if (!g_heap.option.small_size_chunk)
        g_heap.option = init_option();
    if (size < g_heap.option.tiny_size_chunk)
        return alloc(size, &g_heap.tiny_zone);
    if (size < g_heap.option.small_size_chunk)
        return alloc(size, &g_heap.small_zone);
    LOG("BIG ZONE of %d size requested\n", size);
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
}

void *malloc(size_t size) {
    return _malloc(size);
}

void defragment(t_chunk *chunk, t_zone **start_zone, size_t size_zone) {
    t_chunk *to_free = chunk;
    // We check if we are in the same zone before merging two blocks
    if (to_free->next && (void *)to_free->next == (void *)chunk + CHUNK_SIZE(to_free->size) + sizeof(t_chunk) &&
        CHUNK_FREE(to_free->next)) {
        to_free->size += CHUNK_SIZE(to_free->next->size) + sizeof(t_chunk);
        to_free->next = to_free->next->next;
		if (to_free->next)
			to_free->next->prev = to_free->prev;
    }
    if (to_free->prev && (void *)to_free->prev + CHUNK_SIZE(to_free->prev->size) + sizeof(t_chunk) == chunk &&
        CHUNK_FREE(to_free->prev)) {
        t_chunk *prev = to_free->prev;
        prev->next = to_free->next;
        if (to_free->next)
            to_free->next->prev = prev;
        prev->size += CHUNK_SIZE(to_free->size) + sizeof(t_chunk);
        to_free = to_free->prev;
    }

    SET_CHUNK_FREE(to_free);
    if (!(*start_zone)->next)
        return;
    // We have an empty zone to munmap
    if (sizeof(t_chunk) + sizeof(t_zone) + to_free->size == size_zone) {
        // The chunk will be the whole zone => we need to remove it  from link list
        t_chunk *old_prev = to_free->prev;
        if (to_free->prev)
            to_free->prev->next = to_free->next;
        if (to_free->next)
            to_free->next->prev = old_prev;

        void *zone_to_free = (void *)to_free - sizeof(t_zone);
        t_zone *current = *start_zone;
        t_zone *prev = NULL;
        // We remove the zone from the link list
        while (current && current != zone_to_free) {
            prev = current;
            current = current->next;
        }
        if (!prev)
            *start_zone = (*start_zone)->next;
        else
            prev->next = current->next;

        munmap(zone_to_free, size_zone);
        LOG("MUMAP %p start zone %p\n", zone_to_free, *start_zone);
    }
}

void *search_ptr(void *addr, t_chunk *start) {
	t_chunk *tmp = start;
	while (tmp) {
        if (SKIP_HEADER_CHUNK(tmp) == addr)
            return tmp;
        tmp = tmp->next;
    }

    return NULL;
}

void *ft_memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d;
    const unsigned char *s;
    size_t i;

    if (!dest || !src)
        return (dest);
    d = dest;
    s = src;
    i = 0;
    while (i < n) {
        d[i] = s[i];
        i++;
    }
    return (dest);
}

void *realloc(void *addr, size_t size) {
    if (addr == NULL)
        return NULL;
    LOG("REALLOC %p\n", addr);
    void *new_ptr = malloc(size);
    size_t old_size = ((t_chunk *)addr)->size;

    new_ptr = ft_memcpy(new_ptr, SKIP_HEADER_CHUNK(addr), old_size);

    return new_ptr;
}

void free(void *addr) {
    if (addr == NULL)
        return;
    t_chunk *start = NULL;

    if (g_heap.tiny_zone) {
        start = SKIP_HEADER_ZONE(g_heap.tiny_zone);

        t_chunk *found = search_ptr(addr, start);
        if (found) {
            SET_CHUNK_FREE(found);
            defragment(found, &g_heap.tiny_zone, g_heap.option.tiny_size_zone);
            return;
        }
    }
    if (g_heap.small_zone) {
        start = SKIP_HEADER_ZONE(g_heap.small_zone);
        t_chunk *found = search_ptr(addr, start);

        if (found) {
            SET_CHUNK_FREE(found);
            defragment(found, &g_heap.small_zone, g_heap.option.small_size_zone);
            return;
        }
    }
    ft_printf("free () : invalid pointer for %p\n", addr);
    abort();
}

void dump_zone(bool show_data, bool show_header, t_zone *start_zone) {
    t_chunk *current = (t_chunk *)((void *)start_zone + sizeof(t_zone));
    t_chunk *data = 0;
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
            ft_printf("- N %p ", (void *)current + current->size + sizeof(t_chunk));

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
    if (g_heap.tiny_zone)
        dump_zone(show_data, show_header, g_heap.tiny_zone);
    if (g_heap.small_zone)
        dump_zone(show_data, show_header, g_heap.small_zone);
}