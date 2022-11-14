#ifndef scheduling_h
#define scheduling_h
#include <string>
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

#endif
