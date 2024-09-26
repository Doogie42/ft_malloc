#include "malloc.h"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	size_t len = 1256;
	char *test3[1256] = {0};
	for (size_t i = 0; i < len; i++)
	{
		test3[i] = malloc(50);
		for (size_t j = 0; j < 50; j++)
		{
			test3[i][j] = 0x42 + j;
		}

		// printf("%zu: %p\n",i, test3[i]);
		test3[i][2] = 42;
		test3[i][48] = 0;
	}

	// dump_malloc(true, true);
	// for (size_t i = 0; i < len; i++)
	// {
	// 	printf("%zu : %x %p\n",i, test3[i][0], &test3[i][0]);
	// }
	// 	printf("%zu : %x %p\n",len -2, test3[len -2][0], &test3[len -2][0]);

	// dump_malloc(true, true);
	return 0;
	for (size_t i = 0; i < len; i++)
	{
		free(test3[i]);
	}
	// dump_malloc(true, true);

	return 0;

	for (size_t i = 0; i < len; i++)
	{
		test3[i] = malloc(50);
		for (size_t j = 0; j < 50; j++)
		{
			test3[i][j] = 0x42 + j;
		}

		// printf("%zu: %p\n",i, test3[i]);
		test3[i][2] = 42;
		test3[i][48] = 0;
	}

	for (size_t i = 0; i < 10; i++)
	{
		free(test3[i]);
	}
	return 0;
}