/*
	Main.c
*/
#include "main.h"

sem_t *s;

void task1 (void)
{
   s = sem_open();
   int *p1, *p2, *p3, *p4;
   clock_t t1, t2;

   while (1)
   {
      t1 = clock();
      p1 = (int*)malloc(20*sizeof(int));
      p2 = (int*)malloc(30*sizeof(int));
      free (p1);
      p3 = (int*)malloc(10*sizeof(int));
      free (p2);
      p4 = (int*)malloc(40*sizeof(int));
      free (p3);
      free (p4);
      wait(s);
      t2 = clock();
      if (t2-t1 <= 1)
         sleep (2);
   }
}

void task2 (void)
{
   int i=0;
   while (1)
   {
      if (++i%100 == 0)
      {
         i=0;
         signal (s);
      }
      if (time(0) == 2)
         exit (0);
   }
}

void task3 (void)
{
   while (1);
}


/**
  * @brief  Main program.
  * @param  None
  * @retval int
  */
int main (void)
{
   pkernel_boot ((size_t)320, CLOCK, TICK_FREQ);

   pkernel_newprocess (&task1, (size_t)320, 1, 0);
   pkernel_newprocess (&task2, (size_t)320, 1, 0);
   pkernel_newprocess (&task3, (size_t)320, 1, 0);

   pkernel_run ();
   while (1);  // Unreachable.
}
