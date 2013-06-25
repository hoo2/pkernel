/*
	Main.c
*/
#include "main.h"

sem_t *s;

void task1 (void);
void task2 (void);
void task3 (void);
void serv (void);

void task1 (void)
{
   s = sem_open();
   /*
    * Safe to put it here task1
    * defined first --> called first
    */
   int cf=0, *p1, *p2, *p3, *p4;
   clock_t t1, t2;

   p1 = (int*)malloc(20*sizeof(int));
   p2 = (int*)malloc(30*sizeof(int));
   free (p1);
   p3 = (int*)malloc(10*sizeof(int));
   free (p2);
   p4 = (int*)malloc(40*sizeof(int));
   free (p3);
   free (p4);

   while (1)
   {
      t1 = clock();

      if (!cf)
      {
         crontab(&task3, 320, 0, 0, 1, 3, 10);
         //crontab_r (&task3);      // To disable

         microntab(serv, 30);
         //microntab_r(serv);       // To disable
         cf = 1;
      }
      wait(s);    // Suspend if no s
      t2 = clock();
      if (t2-t1 <= 1)
         sleep (2);  // Suspend for 2 ticks
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
   int i;
   for (i=0 ; i<15000 ; ++i)
      ;
   while (1);
}

void serv (void)
{
   int i;
   for (i=0 ; i<10 ; ++i)
      ;
}

/**
  * @brief  Main program.
  * @param  None
  * @retval int
  */
int main (void)
{
   kinit ((size_t)320, CLOCK, TICK_FREQ);

   knew (&task1, (size_t)320, 0, 0);
   knew (&task2, (size_t)320, 1, 0);
   //knew (&task3, (size_t)320, 1, 0);

   krun ();
   while (1);  // Unreachable.
}
