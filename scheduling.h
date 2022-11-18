#ifndef SCHEDULING_H
#define SCHEDULING_H
#include <string>
#include <queue>

using namespace std;

struct Process
{
  string name;
  int arrival;
  int arrived;
  int quota;
  float RR;
  int service;
  int permService;
  int finishTime;
  int turnAround;
  float normTurn;
  int priority;
  char *activity;

  Process(string name, int arrival, int service);
};

struct Schedule
{
  string mode;
  string schedulePolicy;
  string policyName;
  int lastInstance;
  int noOfProcesses;
  int insertedProcesses;
  int quota;
  Process **processes;

  Schedule(string mode, string schedulePolicy, int lastInstance, int noOfProcesses);
  void insertProcess(Process *process);
};

int enqueueArrivals(queue<Process *> *q, int time);
void updateWaiting(queue<Process *> *q, Process *f, int time);
#endif
