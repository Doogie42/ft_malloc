#include "malloc.h"


int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	// char * a = malloc(63);
	// for (size_t i = 0; i < 62; i++)
	// {
	// 	a[i] = 0x42 + i;
	// }

	// char * b = malloc(32);
	// 	for (size_t i = 0; i < 10; i++)
	// {
	// 	b[i] = 0x42 + i;
	// }
	// char * c = malloc(16);
	// for (size_t i = 0; i < 10; i++)
	// {
	// 	c[i] = 0x42 + i;
	// }
	// char * d = malloc(16);
	// for (size_t i = 0; i < 10; i++)
	// {
	// 	d[i] = 0x42 + i;
	// }
	// dump_malloc(true, true);
	// return 1;
	size_t len = 2500;
	char *test3[2500] = {0};
	for (size_t i = 0; i < len; i++)
	{
		size_t size = rand() % 63;
		test3[i] = malloc(size);
		for (size_t j = 0; j < size; j++)
		{
			test3[i][j] = 0x42 + j;
		}

		// printf("%zu: %p\n",i, test3[i]);
		// test3[i][2] = 42;
		// test3[i][48] = 0;
		// LOG("%d : %p\n", i, test3[i]);
	}
	// dump_malloc(false, true);
	// for (size_t i = 0; i < len; i++)
	// {
	// 	printf("%zu : %x %p\n",i, test3[i][0], &test3[i][0]);
	// }
	// 	printf("%zu : %x %p\n",len -2, test3[len -2][0], &test3[len -2][0]);
	for (int i = len - 1; i >= 0; i--)
	{
		free(test3[i]);
	}
	dump_malloc(false, true);
	return 1;
	for (size_t i = 0; i < len; i++)
	{
		test3[i] = malloc(50);
		ft_printf("%d: %p\n",i, test3[i]);

		if (test3[i] == NULL){
			LOG("WHAT ?\n");
			continue;
		}

		for (size_t j = 0; j < 50; j++)
		{
			test3[i][j] = 0x42 + j;
		}

		// test3[i][2] = 42;
		// test3[i][48] = 0;
	}
	// dump_malloc(false, true);

	return 1;

	// for (size_t i = 0; i < 10; i++)
	// {
	// 	free(test3[i]);
	// }
	// dump_malloc(true, true);
	
	return 0;
}