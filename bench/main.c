#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>


#define MAX_CURRENT_ALLOC 20000

const char *header = "start malloc";
const char *footer = "end malloc";

void *set_guard(char *addr, size_t size) {
    strcpy(addr, header);
    strcpy(addr + size - strlen(footer) - 1, footer);
    return addr;
}

int check_guard(char *addr, size_t size) {
    int ret = 1;
    if (strncmp(addr, header, strlen(header))) {
        printf("\n******************************\n");
        printf("WARNING HEADER DIFFERENT for %p size was %zu got :\n", addr, size);
        for (size_t i = 0; i <= strlen(header); i++) {
            if (!addr[i])
                printf("0");
            else
                printf("%c", addr[i]);
        }
        printf("\n******************************\n");
        ret = 0;
    }
    if (strncmp(addr + size - strlen(footer) - 1, footer, strlen(footer))) {
        printf("\n******************************\n");
        printf("WARNING FOOTER DIFFERENT for %p size was %zu got :\n", addr, size);
        for (size_t i = 0; i <= strlen(footer); i++) {
            if (!addr[i])
                printf("0");
            else
                printf("%c", addr[i]);
        }
        printf("\n******************************\n");
        ret = 0;
    }
    if (!ret)
        fflush(NULL);
    return ret;
}

void run(unsigned int max_sim, bool free_all_end) {
    char *ptr[MAX_CURRENT_ALLOC] = {0};

    unsigned int sizealloc[MAX_CURRENT_ALLOC] = {0};
    size_t currentAlloc = 0;
    int nbAlloc = 0;
    int nbFree = 0;
    for (int i = 0; i < max_sim; i++) {
        int idx = rand() % (MAX_CURRENT_ALLOC - 1) + 1;
        if (ptr[idx] == NULL) {
            size_t size = rand() % 600 + 200;

            void *ret = malloc(size);
            ptr[idx] = ret;
            if (!ptr[idx]) {
				printf("WARNING : got a NULL malloc\n");
                continue;
            }
            sizealloc[idx] = size;
            currentAlloc += size;
            ptr[idx] = set_guard(ptr[idx], size);
            nbAlloc++;
        } else {
            check_guard(ptr[idx], sizealloc[idx]);
            free(ptr[idx]);
            ptr[idx] = NULL;
            currentAlloc -= sizealloc[idx];
            nbFree++;
        }
    }
	if (!free_all_end)
		return;
	for (int i = MAX_CURRENT_ALLOC - 1; i >= 0; i--) {
        if (ptr[i]) {
            free(ptr[i]);
        }
    }
}

int main(int argc, char **argv) {
    if (argc == 1)
        srand(42);
    else
        srand(atoi(argv[1]));

    write(1, "1\n", 6);
	run(50000, true);
    write(1, "2\n", 6);
	run(50000, true);
    write(1, "3\n", 6);
    

    // dump_malloc(false, true);
}
