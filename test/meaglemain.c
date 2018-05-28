#include <stdio.h>
#include "pthread.h"

void start_minigui (void *data)
{
  minigui_entry(0, data);
}
void create_minigui(void)
{
  pthread_t th;
  pthread_attr_t new_attr;
  pthread_attr_init (&new_attr);
  pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
  pthread_attr_setstacksize (&new_attr, 1024 * 1024);
  pthread_create(&th, &new_attr, start_minigui, NULL);
  pthread_attr_destroy (&new_attr);
}

