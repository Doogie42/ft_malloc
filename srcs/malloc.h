#pragma once

#define _GNU_SOURCE
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "ft_printf.h"

#include <fcntl.h>

#define DEBUG 0

#define GUARD_CHUNK_SIZE 256

#if DEBUG
	#define LOG(...) ft_printf(__VA_ARGS__)
#else
	#define LOG(...) do{} while(0)
#endif



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
    void *end;
    t_zone *next;
    t_zone *prev;
};

struct heap {
    t_option option;
    t_zone *tiny_zone;
    t_zone *small_zone;
    t_chunk *big_chunk;
};

enum zone_type{
	e_tiny_zone,
	e_small_zone,
	e_big_zone
};

typedef struct zone_info{
	t_zone **zone;
	size_t size_zone;
	size_t size_chunk;
	enum zone_type type;
}t_zone_info;

#define SKIP_HEADER_ZONE(zone) ((void *)zone + sizeof(t_zone))
#define SKIP_HEADER_CHUNK(chunk) ((void *)chunk + sizeof(t_chunk))

#define SKIP_BEGIN_GUARD_CHUNK(zone) ((void *) zone + GUARD_CHUNK_SIZE)

// Because our address is 16 bit aligned the size of each chunk is at least 16 bit
// -> we can use the last bits to store data
// -> rightmost bit will tell us if chunk is free or not -> no more bool :)
#define CHUNK_FREE(x) ((x->size & -x->size) - 1)
// We unset the last bit if set
#define CHUNK_SIZE(x) (size_t)(x & ~1)

#define SET_CHUNK_USED(x) x->size ^= 1
#define SET_CHUNK_FREE(x) x->size &= ~1





void *malloc(size_t size);
void dump_malloc(bool show_data, bool show_header);
void *calloc(size_t nmemb, size_t size);


void *find_zone_ptr(void *addr);
void *search_ptr(void *addr, t_zone *good_zone);
void *defragment_chunk(t_chunk *chunk);