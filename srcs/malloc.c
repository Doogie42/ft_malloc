#include "malloc.h"

struct heap g_heap;

pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

static struct option init_option() {
    struct option option = {
        .tiny_size_zone = sysconf(_SC_PAGESIZE) * 4,
        .tiny_size_chunk = option.tiny_size_zone / 128,
        .small_size_zone = sysconf(_SC_PAGESIZE) * 64,
        .small_size_chunk = option.small_size_zone / 64,
    };
    return option;
}

static void first_call() {
    static bool first = false;

    if (first == false) {
        g_heap.option = init_option();
        g_heap.small_zone = NULL;
        g_heap.tiny_zone = NULL;
        g_heap.big_chunk = NULL;
    }
    first = true;
}

static size_t align_mem(size_t size) {
    unsigned int memalign = 64;
    if (size < memalign)
        size = memalign;
    else if (size % memalign != 0)
        size = size + memalign - size % memalign;
    return size;
}

static t_chunk *big_alloc(size_t size) {
    t_chunk *new_chunk =
        mmap(NULL, size + sizeof(t_chunk), PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANON, -1, 0);
    if (!new_chunk) return NULL;
    new_chunk->next = NULL;
    new_chunk->prev = NULL;
    new_chunk->size = size;
    SET_CHUNK_USED(new_chunk);

    if (!g_heap.big_chunk) {
        g_heap.big_chunk = new_chunk;
        return SKIP_HEADER_CHUNK(new_chunk);
    }
    t_chunk *first_chunk = g_heap.big_chunk;
    g_heap.big_chunk = new_chunk;
    g_heap.big_chunk->next = first_chunk;
    first_chunk->prev = g_heap.big_chunk;

    return SKIP_HEADER_CHUNK(new_chunk);
}

static t_chunk *find_free_chunk(t_chunk *start, size_t size) {
    t_chunk *current = start;

    while (current) {
        if (CHUNK_FREE(current) && CHUNK_SIZE(current->size) >= size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

t_chunk *split_free_chunk(t_chunk *free_chunk, size_t size) {
    if (CHUNK_SIZE(free_chunk->size) <= size + sizeof(t_chunk)) {
        SET_CHUNK_USED(free_chunk);
        return free_chunk;
    }
    t_chunk *old_next = free_chunk->next;
    size_t new_free_size = free_chunk->size - size - sizeof(t_chunk);

    t_chunk *used_chunk = free_chunk;
    free_chunk = SHIFT_FORWARD_CHUNK(free_chunk, size + sizeof(t_chunk));

    used_chunk->size = size;
    free_chunk->size = new_free_size;

    used_chunk->next = free_chunk;
    free_chunk->next = old_next;

    old_next->prev = free_chunk;
    free_chunk->prev = used_chunk;

    SET_CHUNK_FREE(free_chunk);
    SET_CHUNK_USED(used_chunk);
    return used_chunk;
}

static void *alloc(size_t size) {
    t_zone_info zone_info = get_zone_from_size(size);
    if (zone_info.type == e_big_zone) {
        return big_alloc(size);
    }
    if (*zone_info.zone == NULL) {
        t_zone *new_zone = init_zone(zone_info);
        if (!new_zone) return NULL;
        *zone_info.zone = new_zone;
    }

    t_chunk *free_chunk =
        find_free_chunk(SKIP_HEADER_ZONE(*zone_info.zone), size);
    if (!free_chunk) {
        t_zone *new_zone = init_zone(zone_info);
        if (!new_zone) return NULL;
        *zone_info.zone = link_zone(*zone_info.zone, new_zone);
        free_chunk = find_free_chunk(SKIP_HEADER_ZONE(*zone_info.zone), size);
    }
    if (!free_chunk) return NULL;
    t_chunk *ret_chunk = split_free_chunk(free_chunk, size);

    return SKIP_HEADER_CHUNK(ret_chunk);
}

void *internal_malloc(size_t size) {
    size = align_mem(size);
    first_call();
    char *ptr = alloc(size);
    return ptr;
}

void *malloc(size_t size) {
    pthread_mutex_lock(&g_mutex);
    void *ptr = internal_malloc(size);
    pthread_mutex_unlock(&g_mutex);
    return ptr;
}
