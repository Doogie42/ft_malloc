#pragma once

#define _GNU_SOURCE
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "ft_printf.h"

#include <fcntl.h>

#define DEBUG 1

#if DEBUG
	#define LOG(...) ft_printf(__VA_ARGS__)
#else
	#define LOG(...) do{} while(0)
#endif

extern void* malloc(size_t size);
extern void free(void *addr);
extern void dump_malloc(bool show_data, bool show_header);

