#include "malloc.h"

struct heap g_heap;

struct option init_option() {
    struct option option = {
        .tiny_size_zone = sysconf(_SC_PAGESIZE) * 4,
        .tiny_size_chunk = option.tiny_size_zone / 128 + 1,
        .small_size_zone = sysconf(_SC_PAGESIZE) * 32,
        .small_size_chunk = option.small_size_zone / 128 + 1,
    };
    return option;
}


size_t align_mem(size_t size) {
    unsigned int memalign = 16;
    if (size < memalign)
        size = memalign;
    else if (size % memalign != 0)
        size = size + memalign - size % memalign;
    return size;
}


t_zone_info get_zone_from_size(size_t size)
{
	t_zone_info zone_info;
	if (size <= g_heap.option.tiny_size_chunk){
		zone_info.size_chunk = g_heap.option.tiny_size_chunk;
		zone_info.zone = &g_heap.tiny_zone;
		zone_info.type = e_tiny_zone;
		zone_info.size_zone = g_heap.option.tiny_size_zone;
	}else if (size <= g_heap.option.small_size_chunk){
		zone_info.size_chunk = g_heap.option.small_size_chunk;
		zone_info.zone = &g_heap.small_zone;
		zone_info.type = e_small_zone;
		zone_info.size_zone = g_heap.option.small_size_zone;

	}
	else{
		zone_info.type = e_big_zone;
	}
	return zone_info;
}


void *create_new_zone(const t_zone_info zone_info)
{
    t_zone *new_zone = mmap(NULL, zone_info.size_zone, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	new_zone->next = NULL;
	new_zone->prev = NULL;
	new_zone->end = (void *) new_zone + zone_info.size_zone;
	return new_zone;
}

void* set_up_new_zone(t_zone *new_zone, const size_t zone_size)
{
	t_chunk *guard_chunk_begin = SKIP_HEADER_CHUNK(new_zone);
	t_chunk *guard_chunk_end = (void *) new_zone->end - GUARD_CHUNK_SIZE;
	t_chunk *free_chunk = SKIP_BEGIN_GUARD_CHUNK(new_zone);

	guard_chunk_begin->size = GUARD_CHUNK_SIZE - sizeof(t_chunk);
	guard_chunk_end->size = GUARD_CHUNK_SIZE - sizeof(t_chunk);
	free_chunk->size = zone_size - sizeof(t_chunk) - sizeof(t_zone) - 2 * GUARD_CHUNK_SIZE;
	guard_chunk_begin->next = free_chunk;
	guard_chunk_begin->prev = NULL;
	free_chunk->prev = guard_chunk_begin;
	free_chunk->next = guard_chunk_end;
	guard_chunk_end->next = NULL;
	guard_chunk_end->prev = free_chunk;
	SET_CHUNK_FREE(free_chunk);
	SET_CHUNK_USED(guard_chunk_begin);
	SET_CHUNK_USED(guard_chunk_end);
	return new_zone;
}

void *find_free_chunk(const t_zone *zone, const size_t size)
{
	t_chunk *current = SKIP_BEGIN_GUARD_CHUNK(zone);

	while (current){
		if (CHUNK_FREE(current) && CHUNK_SIZE(current->size) >= size + sizeof(t_chunk))
			return current;
		current = current->next;
	}
	return current;
}


void *split_chunk(t_chunk *free_chunk, const size_t size, const t_zone_info zone_info)
{
	ssize_t 	 new_free_size = CHUNK_SIZE(free_chunk->size) - sizeof(t_chunk) - size;
	// We dont want very small chunk that we wont be able to allocate later
	// minum size chunk we have
	if (new_free_size < (ssize_t) zone_info.size_chunk){
		SET_CHUNK_USED(free_chunk);
		return free_chunk;
	}
	t_chunk *old_next = free_chunk->next;
	t_chunk *new_chunk = free_chunk;
	new_chunk->size = size;
	free_chunk = (void *) free_chunk + size + sizeof(t_chunk);
	free_chunk->prev = new_chunk;
	free_chunk->next = old_next;
	free_chunk->size = new_free_size;
	old_next->prev = free_chunk;
	new_chunk->next = free_chunk;

	SET_CHUNK_FREE(free_chunk);
	SET_CHUNK_USED(new_chunk);
	return new_chunk;
}

void link_new_zone(t_zone_info *zone_info, t_zone *new_zone)
{
	t_zone *current = *(zone_info->zone);
	while (current->next)
	{
		current = current->next;
	}
	current->next = new_zone;
	new_zone->prev = current;

	t_chunk *guard_chunk_begin_new_zone = SKIP_HEADER_ZONE(new_zone);
	t_chunk *guard_chunk_end_last_zone = (void *) current->end - GUARD_CHUNK_SIZE;
	guard_chunk_begin_new_zone->prev = guard_chunk_end_last_zone;
	guard_chunk_end_last_zone->next = guard_chunk_begin_new_zone;
}

void *alloc(const size_t size)
{
	t_zone_info zone_info = get_zone_from_size(size);

	if (*zone_info.zone == NULL){
		*zone_info.zone = create_new_zone(zone_info);
		*zone_info.zone = set_up_new_zone(*zone_info.zone, zone_info.size_zone);
	}
	t_chunk *suitable_chunk = find_free_chunk(*zone_info.zone, size);
	if (!suitable_chunk){
		t_zone *new_zone = create_new_zone(zone_info);
		new_zone = set_up_new_zone(new_zone, zone_info.size_zone);
		link_new_zone(&zone_info, new_zone);
		suitable_chunk = find_free_chunk(new_zone, size);
	}
	
	t_chunk *malloced_chunk = split_chunk(suitable_chunk, size, zone_info);
	return SKIP_HEADER_CHUNK(malloced_chunk);
}

void first_call()
{
	static bool first = false;
	if (first == false){
		g_heap.option = init_option();
		g_heap.small_zone = NULL;
		g_heap.tiny_zone = NULL;
	    g_heap.big_chunk = NULL;

	}
	first = true;
}


void *malloc(size_t size)
{
	first_call();
	size = align_mem(size);
	void *ptr = alloc(size);
	return ptr;
}

