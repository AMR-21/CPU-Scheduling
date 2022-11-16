#include "scheduling.h"
#include "FCFS.h"

extern queue<Process *> q;
extern Schedule *schedule;

void FCFS()
{
  for (int j = 0; j < schedule->lastInstance; j++)
  {
    enqueueArrivals(&q, j);

    // Dispatch front
    Process *f = q.front();
    if (f)
    {
      f->activity[j] = '*';
      f->service--;

      // Front finishes service
      if (!f->service)
      {
        f->finishTime = j + 1;
        q.pop();
      }
    }

    updateWaiting(&q, f, j);
  }
}