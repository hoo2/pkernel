/*
	Main.c
*/
#include "main.h"

sem_t *s;

int task1 (void)
{
   s = sem_open();
   int *p1, *p2, *p3, *p4;
   clock_t t1, t2;

   while (1)
   {
      t1 = clock();
      p1 = (int*)malloc(20*sizeof(int));
         wait (s);
      p2 = (int*)malloc(30*sizeof(int));
         wait (s);
      free (p1);
         wait (s);
      p3 = (int*)malloc(10*sizeof(int));
         wait (s);
      free (p2);
         wait (s);
      p4 = (int*)malloc(40*sizeof(int));
         wait (s);
      free (p3);
         wait (s);
      free (p4);
         wait (s);
      t2 = clock();
      if (t2-t1 <= 1)
         sleep (2);
   }
}

int task2 (void)
{
   int i=0;
   while (1)
   {
      if (++i%100 == 0)
      {
         i=0;
         signal (s);
      }
   }
}
/**
  * @brief  Main program.
  * @param  None
  * @retval int
  */
int main (void)
{
   pkernel_boot (320, CLOCK, TICK_FREQ);

   pkernel_newprocess (&task1, (size_t)320, 1, 0);
   pkernel_newprocess (&task2, (size_t)320, 1, 0);

   pkernel_run ();
   while (1);  // Unreachable.
}
