
#include <stdlib.h>

int      main()
{
   int   i;
   char  *addr;

   i = 0;
   while (i < 50)
   {
      addr = (char*)malloc(50);
      addr[0] = 42;
      i++;
   }
   return (0);
}