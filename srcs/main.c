// #include "malloc.h"

// int main()
// {
// 	char *abc[800];
// 	for (size_t i = 0; i < 800; i++)
// 	{
// 		abc[i] = malloc(38);
// 		/* code */
// 	}
// 	for (size_t i = 0; i < 800; i++)
// 	// for (int i = 800 - 1; i >= 0; i--)
// 	{
// 		free(abc[i]);
// 	}
// 	char * a = malloc(16);
// 	char * b = malloc(16);
// 	memset(a, 0x42, 16);
// 	memset(b, 0x42, 16);
// 	free(a);
// 	free(b);
// 	a = malloc(48);
// 	b = malloc(32);
// 	memset(a, 0x42, 48);
// 	free(a);
// 	a = malloc(16);
// 	memset(a, 0x42, 16);
// 	free(a);
// 	a = malloc(68);
// 	memset(a, 0x42, 68);

// 	memset(b, 0x42, 32);

// 	dump_malloc(false, true);
// 	return 0;
// }


#include <unistd.h>
// #include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "malloc.h"


#define MAX_CURRENT_ALLOC 20000

int main(int argc, char **argv)
{
	if (argc == 1)
		srand(42);
	else
		srand(atoi(argv[1]));
	char *ptr[MAX_CURRENT_ALLOC] = {0};
	for (size_t i = 0; i < MAX_CURRENT_ALLOC; i++)
	{
		ptr[i] = NULL;
	}
	
	unsigned int sizealloc[MAX_CURRENT_ALLOC] = {0};
	size_t currentAlloc = 0;
	int max_sim = 100000;
	write(1, "STARTING\n", 10);
	int nbfree = 0;
	for (int i = 0; i < max_sim; i++)
	{
		// printf("%d\n", i);
		int idx = rand() % MAX_CURRENT_ALLOC;
		if (idx == MAX_CURRENT_ALLOC)
			idx = MAX_CURRENT_ALLOC - 1;
		if (ptr[idx] == NULL){
		// printf("%d\n", i);

			size_t size = rand() % 612 + 1;
			ptr[idx] = calloc(size, 1);
			if (!ptr[idx])
				continue;
			memset(ptr[idx], 42, size - 1);
			sizealloc[idx] = size;
			currentAlloc += size;
		}
		else{
			nbfree ++;
			free (ptr[idx]);
			ptr[idx] = NULL;
			currentAlloc -= sizealloc[idx];

		}
	}
	write(1, "DONE\n", 6);
	int nbNoFree = 10;
			// for (size_t i = 0; i < MAX_CURRENT_ALLOC; i++)
	for (int i = MAX_CURRENT_ALLOC - 1; i >= 0; i--)
	{
		if (ptr[i]){
			// if (i % 12 && nbNoFree > 0){
			// 	nbfree --;
			// 	continue;
			// }
				free(ptr[i]);
		}
	}
	
	printf("Max alloced %zu max free %d\n", currentAlloc, nbfree);
	dump_malloc(false, true);

}