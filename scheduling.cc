#include "scheduling.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>
#include <queue>
// #include <bits/stdc++.h>
#include <algorithm>
#include <string.h>

#define DEFAULT_BUF_LENGTH 20480

using namespace std;
Schedule *schedule;
int count = 0;
vector<Schedule *> schedules;
queue<Process *> q;

Schedule::Schedule(string mode, string schedulePolicy, int lastInstance, int noOfProcesses)
{

  Schedule::mode = mode;
  Schedule::schedulePolicy = schedulePolicy;
  Schedule::lastInstance = lastInstance;
  Schedule::noOfProcesses = noOfProcesses;
  Schedule::insertedProcesses = 0;
  Schedule::quota = 1;

  if (schedulePolicy == "1")
    Schedule::policyName = "FCFS";
  else if (schedulePolicy[0] == '2')
    Schedule::policyName = "RR" + schedulePolicy.substr(1, schedulePolicy.length() - 1);
  // else if (schedulePolicy == "2-4")
  // Schedule::policyName = "RR-4";
  else if (schedulePolicy == "3")
    Schedule::policyName = "SPN";
  else if (schedulePolicy == "4")
    Schedule::policyName = "SRT";
  else if (schedulePolicy == "5")
    Schedule::policyName = "HRRN";
  // ADD POLICY HERE

  Schedule::processes = (Process **)
      malloc(noOfProcesses * sizeof(Process *));
}

void Schedule::insertProcess(Process *process)
{
  if (schedule->insertedProcesses < schedule->noOfProcesses)
    schedule->processes[schedule->insertedProcesses] = process;

  schedule->insertedProcesses++;
}

Process::Process(string name, int arrival, int service)
{
  Process::name = name;
  Process::arrival = arrival;
  Process::service = service;
  Process::permService = service;
  Process::arrived = 0;
  Process::quota = 0;
  Process::RR = 0;
  Process::activity = (char *)malloc(schedule->lastInstance * sizeof(char));
}

void showq(queue<Process *> gq)
{
  queue<Process *> g = gq;
  while (!g.empty())
  {
    cout << '\t' << g.front()->name;
    g.pop();
  }
  cout << '\n';
}

void clear(queue<Process *> q)
{
  queue<Process *> empty;
  swap(q, empty);
}

bool comparePermService(Process *p1, Process *p2)
{
  return (p1->permService < p2->permService);
}

void sortSPN()
{
  vector<Process *> procs;
  while (!q.empty())
  {
    procs.push_back(q.front());
    q.pop();
  }

  sort(procs.begin(), procs.end(), comparePermService);

  for (Process *p : procs)
    q.push(p);
}

bool compareService(Process *p1, Process *p2)
{
  return (p1->service < p2->service);
}

void sortSRT()
{
  vector<Process *> procs;
  while (!q.empty())
  {
    procs.push_back(q.front());
    q.pop();
  }

  sort(procs.begin(), procs.end(), compareService);

  for (Process *p : procs)
    q.push(p);
}

bool compareRR(Process *p1, Process *p2)
{
  return (p1->RR > p2->RR);
}

void sortRR(int time)
{
  vector<Process *> procs;
  while (!q.empty())
  {
    Process *p = q.front();
    p->RR = ((time - p->arrival) + p->permService) * 1.0 / p->permService;
    procs.push_back(p);
    q.pop();
  }

  sort(procs.begin(), procs.end(), compareRR);

  for (Process *p : procs)
    q.push(p);
}

void printTrace()
{
  printf("%-6s", schedule->policyName.c_str());
  for (int i = 0; i <= schedule->lastInstance; i++)
    printf("%d ", i % 10);
  printf("\n");
  for (int j = 0; j <= schedule->lastInstance * 2 + 7; j++)
    printf("-");
  printf("\n");

  for (int i = 0; i < schedule->noOfProcesses; i++)
  {
    Process *p = schedule->processes[i];
    printf("%-6s", p->name.c_str());
    printf("|");
    for (int j = 0; j < schedule->lastInstance; j++)
    {
      printf("%c", p->activity[j] ? p->activity[j] : ' ');
      printf("|");
    }
    printf(" ");
    printf("\n");
  }

  for (int j = 0; j <= schedule->lastInstance * 2 + 7; j++)
    printf("-");
  printf("\n");
  printf("\n");
}

void printStats()
{
  printf("%s\n", schedule->policyName.c_str());
  printf("Process    |");
  for (int i = 0; i < schedule->noOfProcesses; i++)
  {
    Process *p = schedule->processes[i];
    printf("%*s%*s", 3, p->name.c_str(), 2, "");
    printf("|");
  }
  printf("\n");

  printf("Arrival    |");
  for (int i = 0; i < schedule->noOfProcesses; i++)
  {
    Process *p = schedule->processes[i];
    printf("%*d%*s", 3, p->arrival, 2, "");
    printf("|");
  }
  printf("\n");

  printf("Service    |");
  for (int i = 0; i < schedule->noOfProcesses; i++)
  {
    Process *p = schedule->processes[i];
    printf("%*d%*s", 3, p->permService, 2, "");
    printf("|");
  }
  printf(" Mean|");
  printf("\n");

  printf("Finish     |");
  for (int i = 0; i < schedule->noOfProcesses; i++)
  {
    Process *p = schedule->processes[i];
    printf("%*d%*s", 3, p->finishTime, 2, "");
    printf("|");
  }
  printf("-----|\n");

  printf("Turnaround |");
  int sumI = 0;
  for (int i = 0; i < schedule->noOfProcesses; i++)
  {
    Process *p = schedule->processes[i];
    p->turnAround = p->finishTime - p->arrival;
    printf("%*d%*s", 3, p->turnAround, 2, "");
    printf("|");
    sumI += p->turnAround;
  }
  printf("%*.2f%*s|", 5, (sumI * 1.0) / schedule->noOfProcesses, 0, "");
  printf("\n");

  printf("NormTurn   |");
  float sumF = 0;
  for (int i = 0; i < schedule->noOfProcesses; i++)
  {
    Process *p = schedule->processes[i];
    p->normTurn = (p->turnAround * 1.0) / p->permService;
    printf(" %*.2f%*s", 1, p->normTurn, 0, "");
    printf("|");
    sumF += p->normTurn;
  }
  printf(" %.2f|\n\n", sumF / schedule->noOfProcesses);
}

int checkForArrival(int currentTime, int arrival)
{
  return currentTime == arrival ? 1 : 0;
}

void updateWaiting(Process *f, int time)
{
  // Update activity of ready processes
  queue<Process *> temp;
  while (!q.empty())
  {
    Process *x = q.front();
    q.pop();
    // Do noting if process is currently active
    if (x == f)
    {
      temp.push(x);
      continue;
    }

    x->activity[time] = '.';
    temp.push(x);
  }

  q = temp;
}

void enqueueArrivals(int time)
{
  // Enqueue arrivals
  for (int i = 0; i < schedule->noOfProcesses; i++)
  {
    Process *p = schedule->processes[i];
    if (checkForArrival(time, p->arrival))
      if (!p->arrived)
      {
        p->arrived = 1;
        q.push(p);
      }
  }
}
/****************************************/
/*              ALGORITHMS              */
/****************************************/

void FCFS()
{
  for (int j = 0; j < schedule->lastInstance; j++)
  {
    enqueueArrivals(j);

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

    updateWaiting(f, j);
  }
}

void RR()
{
  for (int j = 0; j < schedule->lastInstance; j++)
  {
    enqueueArrivals(j);

    Process *f = q.front();
    if (f)
    {
      f->activity[j] = '*';
      f->service--;
      f->quota++;

      if (f->quota == schedule->quota || !f->service)
      {
        f->quota = 0;
        q.pop();
      }

      updateWaiting(f, j);

      // Enqueuing again to stick with given policy of enqueuing
      enqueueArrivals(j + 1);

      if (f->service > 0 and !f->quota)
        q.push(f);

      else
        f->finishTime = j + 1;
    }
    else
      updateWaiting(f, j);
  }
}

void SPN()
{
  for (int j = 0; j < schedule->lastInstance; j++)
  {
    enqueueArrivals(j);

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
        sortSPN();
      }
    }

    updateWaiting(f, j);
  }
}

void SRT()
{
  for (int j = 0; j < schedule->lastInstance; j++)
  {
    enqueueArrivals(j);
    sortSRT();

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
    updateWaiting(f, j);
  }
}

void HRRN()
{
  for (int j = 0; j < schedule->lastInstance; j++)
  {
    enqueueArrivals(j);

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
        sortRR(j);
      }
    }

    updateWaiting(f, j);
  }
}
/****************************************/
/*              ALGORITHMS              */
/****************************************/

void init()
{
  char buf[DEFAULT_BUF_LENGTH];
  cin.read(buf, sizeof(buf));
  int size = cin.gcount();

  if (!size)
  {
    cout << "No such file\n";
    exit(0);
  }

  string inp(buf);

  // Vector of string to save tokens
  vector<string> tokens;

  // stringstream class check1
  stringstream check1(inp);
  string intermediate;

  while (getline(check1, intermediate, '\n'))
  {
    tokens.push_back(intermediate);
  }

  vector<string> polTokens;

  if (tokens[1].find(',') != string::npos)
  {

    // stringstream class check2
    stringstream check2(tokens[1]);

    while (getline(check2, intermediate, ','))
    {
      polTokens.push_back(intermediate);
    }
  }
  else
  {
    schedule = new Schedule(tokens[0], tokens[1], stoi(tokens[2]), stoi(tokens[3]));

    // Round-robin quota
    if (tokens[1][0] == '2')
      schedule->quota = stoi(tokens[1].substr(2, tokens[1].length() - 1));

    schedules.push_back(schedule);
  }

  if (!polTokens.size())
  {
    for (int i = 4; i < tokens.size(); i++)
    {
      string args[3], token;

      stringstream stream(tokens[i]);
      int j = 0;

      while (getline(stream, token, ','))
        args[j++] = token;

      j = 0;

      Process *p = new Process(args[0], stoi(args[1]), stoi(args[2]));
      schedules[0]->insertProcess(p);
    }
  }

  else
  {
    for (string policy : polTokens)
    {
      schedule = new Schedule(tokens[0], policy, stoi(tokens[2]), stoi(tokens[3]));

      if (policy[0] == '2')
        schedule->quota = stoi(policy.substr(2, policy.length() - 1));

      for (int i = 4; i < tokens.size(); i++)
      {
        string args[3], token;

        stringstream stream(tokens[i]);
        int j = 0;
        while (getline(stream, token, ','))
          args[j++] = token;

        j = 0;

        Process *p = new Process(args[0], stoi(args[1]), stoi(args[2]));
        schedule->insertProcess(p);
      }

      schedules.push_back(schedule);
    }
  }
}

int main()
{
  init();

  // Driver
  for (Schedule *s : schedules)
  {
    schedule = s;

    if (s->policyName == "FCFS")
      FCFS();
    else if (s->policyName.substr(0, 2) == "RR")
      RR();
    else if (s->policyName == "SPN")
      SPN();
    else if (s->policyName == "SRT")
      SRT();
    else if (s->policyName == "HRRN")
      HRRN();

    // ADD ALGORITHMS ELSE IF HERE

    if (s->mode == "trace")
      printTrace();
    else
      printStats();

    clear(q);
  }
}