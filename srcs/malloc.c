#include "malloc.h"

typedef struct chunk t_chunk;

struct chunk
{
	size_t size;
	t_chunk *next;
	t_chunk *prev;
	bool free;
};

typedef struct option
{
	size_t tiny_size_zone;
	size_t tiny_size_chunk;
	size_t small_size_zone;
	size_t small_size_chunk;
} t_option;

typedef struct _zone
{
	void *start;
	void *end;
	void *next;
} t_zone;

struct heap
{
	t_option option;
	t_zone *tiny_zone;
	t_zone *small_zone;
};

struct option init_option()
{
	struct option option = {
		.tiny_size_zone = sysconf(_SC_PAGESIZE) * 4,
		.tiny_size_chunk = option.tiny_size_zone / 256,
		.small_size_zone = sysconf(_SC_PAGESIZE) * 64,
		.small_size_chunk = option.small_size_zone / 128,
	};
	return option;
}

#define SKIP_HEADER_ZONE(zone) ((void *)zone + sizeof(t_zone))
#define SKIP_HEADER_CHUNK(chunk) ((void *)chunk + sizeof(t_chunk))

struct heap g_heap;

// void clean_up(void) __attribute__((destructor));
void clean_up(void)
{
	t_zone *next;
	while (g_heap.small_zone)
	{
		next = g_heap.small_zone->next;
		munmap(g_heap.small_zone, g_heap.small_zone->end - g_heap.small_zone->start);
		g_heap.small_zone = next;
		break;
	}

	while (g_heap.tiny_zone)
	{
		next = g_heap.tiny_zone->next;
		munmap(g_heap.tiny_zone, g_heap.small_zone->end - g_heap.small_zone->start);
		g_heap.tiny_zone = next;
	}
}

// void init_zone(void) __attribute__((constructor));
void init_zone(void)
{
	LOG("Size of chunk %d\n", sizeof(t_chunk));
	g_heap.option = init_option();
	g_heap.small_zone = mmap(NULL, g_heap.option.small_size_zone, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	g_heap.small_zone->next = NULL;
	g_heap.small_zone->end = (void *)g_heap.small_zone + g_heap.option.small_size_zone;

	g_heap.tiny_zone = mmap(NULL, g_heap.option.tiny_size_zone, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	g_heap.tiny_zone->next = NULL;
	g_heap.tiny_zone->end = (void *)g_heap.tiny_zone + g_heap.option.tiny_size_zone;
	LOG("TINY ZONE START %p END %p SIZE %d SIZE_CHUNK %d \n", g_heap.tiny_zone, g_heap.tiny_zone->end,
		g_heap.option.tiny_size_zone, g_heap.option.tiny_size_chunk);
	LOG("SMALL ZONE START %p END %p SIZE %d SIZE_CHUNK %d\n", g_heap.small_zone, g_heap.small_zone->end,
		g_heap.option.small_size_zone, g_heap.option.small_size_chunk);
}

t_zone *create_new_zone(size_t size)
{
	t_zone *new_zone = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	LOG("new zone created %p with size %d\n", new_zone, size);
	new_zone->next = NULL;
	new_zone->end = (void *)new_zone + size;
	return new_zone;
}

t_chunk *create_chunk(t_chunk *chunk, t_chunk *prev, size_t size, t_zone *zone)
{
	while ((void *)SKIP_HEADER_CHUNK(chunk) + size > (void *)zone->end || (void *)chunk < (void *)zone)
	{
		if (zone->next)
		{
			zone = zone->next;
		}
		else
		{
			zone->next = create_new_zone(size < g_heap.option.tiny_size_chunk ? g_heap.option.tiny_size_zone : g_heap.option.small_size_zone);
			zone = zone->next;
			zone->next = NULL;
			chunk = SKIP_HEADER_CHUNK(zone);
			prev->next = chunk;
			break;
		}
	}
	chunk->size = size;
	chunk->free = false;
	chunk->prev = prev;

	if (!chunk->next)
	{
		chunk->next = (void *)chunk + chunk->size + sizeof(t_chunk);
		chunk->next->free = true;
	}
	return chunk;
}

void *alloc(size_t size, t_zone *zone)
{
	t_chunk *start = SKIP_HEADER_ZONE(zone);
	t_chunk *prev = NULL;
	if (!start->next)
	{
		start = create_chunk(start, NULL, size, zone);
		return SKIP_HEADER_CHUNK(start);
	}
	while (start->free == false)
	{
		if (!start->next)
			break;
		prev = start;
		start = start->next;
	}
	start = create_chunk(start, prev, size, zone);
	start->prev = prev;

	return SKIP_HEADER_CHUNK(start);
}

void *_malloc(size_t size)
{
	if (!g_heap.small_zone)
		init_zone();

	if (size < g_heap.option.tiny_size_chunk)
		return alloc(size, g_heap.tiny_zone);
	if (size < g_heap.option.small_size_chunk)
		return alloc(size, g_heap.small_zone);
	LOG("BIG ZONE of %d size requested\n", size);
	return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
}

void *malloc(size_t size)
{
	return _malloc(size);
}

void defragment(t_chunk *chunk)
{
	t_chunk *to_free = chunk;
	to_free->free = true;
	if (to_free->prev && to_free->prev->free == true)
	{
		to_free->prev->next = to_free->next;
		to_free->next->prev = to_free->prev;
		to_free->prev->size += to_free->size + sizeof(t_chunk);
	}
}

void *search_ptr(void *addr, t_chunk *start)
{
	while (start)
	{
		if (SKIP_HEADER_CHUNK(start) == addr)
		{
			return start;
		}
		start = start->next;
	}
	return NULL;
}

void free(void *addr)
{
	t_chunk *start = SKIP_HEADER_ZONE(g_heap.tiny_zone);

	t_chunk *found = search_ptr(addr, start);
	if (!found)
	{
		start = SKIP_HEADER_ZONE(g_heap.small_zone);
		found = search_ptr(addr, start);
	}
	if (!found)
	{
		ft_printf("free () : invalid pointer");
		abort();
	}
	found->free = true;
	defragment(found);
}

void dump_malloc(bool show_data, bool show_header)
{
	t_chunk *current = (t_chunk *)((__uint8_t *)g_heap.tiny_zone + sizeof(t_zone));
	t_chunk *data = 0;
	while (current)
	{
		if (current->size == 0)
			break;

		if (show_header)
			data = current;
		else
			data = SKIP_HEADER_CHUNK(current);

		LOG("%p ", current);
		LOG("- %p ", current->next);
		LOG(" : %d bytes", current->size);
		LOG(" free %s", current->free ? "true" : "false");
		if (show_data)
		{
			LOG("\n DUMP:");
			size_t size = current->size;
			if (show_header)
				size += sizeof(t_chunk);

			for (size_t i = 0; i < size; i++)
			{
				char *d = (char *)data;
				LOG("%x ", d[i]);
			}
		}
		LOG("\n");
		current = current->next;
	}
}
