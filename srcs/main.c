#include "malloc.h"


int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	char * a = malloc(10);
	for (size_t i = 0; i < 10; i++)
	{
		a[i] = 0x42 + i;
	}
	char * b = malloc(10);
		for (size_t i = 0; i < 10; i++)
	{
		b[i] = 0x42 + i;
	}
	char * c = malloc(10);
		for (size_t i = 0; i < 10; i++)
	{
		c[i] = 0x42 + i;
	}
	dump_malloc(true, true);

	size_t len = 20;
	char *test3[20] = {0};
	for (size_t i = 0; i < len; i++)
	{
		test3[i] = malloc(25);
		for (size_t j = 0; j < 25; j++)
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
	for (size_t i = 0; i < len; i++)
	{
		free(test3[i]);
	}


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
	dump_malloc(true, true);
	
	return 0;
}