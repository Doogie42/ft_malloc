#pragma once

#define _GNU_SOURCE
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "ft_printf.h"

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

typedef struct zone t_zone;
struct zone {
    char *end;
    t_zone *next;
    t_zone *prev;
};

struct heap {
    t_option option;
    t_zone *tiny_zone;
    t_zone *small_zone;
    t_chunk *big_chunk;
};

enum zone_type { e_tiny_zone, e_small_zone, e_big_zone };

typedef struct zone_info {
    t_zone **zone;
    size_t size_zone;
    size_t size_chunk;
    enum zone_type type;
    size_t smallest_chunk_size;
} t_zone_info;

#define GUARD_CHUNK_SIZE (256 + sizeof(t_chunk))

#define SKIP_HEADER_ZONE(zone) ((t_chunk *)((char *)zone + sizeof(t_zone)))
#define SKIP_HEADER_CHUNK(chunk) ((t_chunk *)((char *)chunk + sizeof(t_chunk)))
#define MEM_TO_CHUNK(addr) (t_chunk *)((char *)addr - sizeof(t_chunk))

#define SKIP_BEGIN_GUARD_CHUNK(zone) \
    ((t_chunk *)((char *)SKIP_HEADER_ZONE(zone) + GUARD_CHUNK_SIZE))
#define GET_END_GUARD(zone) ((t_chunk *)((char *)zone->end - GUARD_CHUNK_SIZE))

// Because our address is 16 bit aligned the size of each chunk is at least 16
// bit
// -> we can use the last bits to store data
// -> rightmost bit will tell us if chunk is free or not
#define CHUNK_FREE(x) ((x->size & -x->size) - 1)
// We unset the last bit if set
#define CHUNK_SIZE(x) (size_t)(x & ~1)

#define SET_CHUNK_USED(x) x->size ^= 1
#define SET_CHUNK_FREE(x) x->size &= ~1

#define CHECK_CHUNK_NEXT_INVALID(chunk) \
    (chunk->next && chunk->next->prev != chunk)
#define SHIFT_FORWARD_CHUNK(chunk, size) (t_chunk *)((char *)chunk + size)
#define SHIFT_BACKARD_CHUNK(chunk, size) (t_chunk *)((char *)chunk - size)

#define my_export __attribute__((visibility("default")))

my_export void *malloc(size_t size);
my_export void show_alloc_mem(bool show_data, bool show_header);
my_export void *calloc(size_t nmemb, size_t size);
my_export void *realloc(void *ptr, size_t size);
my_export void free(void *ptr);

t_zone *find_zone_ptr(void *addr);
void internal_free(void *addr);
void *internal_malloc(size_t size);

t_zone_info get_zone_from_size(size_t size);
t_zone *init_zone(t_zone_info zone_info);
t_zone *link_zone(t_zone *first_zone, t_zone *new_zone);
t_chunk *split_free_chunk(t_chunk *free_chunk, size_t size);
