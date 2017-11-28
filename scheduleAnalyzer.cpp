//------------------------------------------------------------------------------
// Programmer: Charles Gaitley         Date: 11/22/2017
// Class: CS 3800                      Section: 1:00 PM
// File: scheduleAnalyzer.cpp
// Description: This File Will Contain The main() Function And The Program Will
//              Compare Different Scheduling Algorithms Used In Operating
//              Systems.
//------------------------------------------------------------------------------

#include "Process.h"

// Run Different Scheduling Algorithms And Compare Their Results
int main(int argc, char *argv[])
{ 

  // Local Constants
  const char YES = 'y', GENERATE = 'y';
  
  const unsigned int FEEDBACK_LEVELS = 4, FEEDBACK_OFFSET = 2;

  // Local Objects And Variable
  deque<Process> newProcesses;
  
  mutex lock;
  
  thread fifo, spn, srt, rr, fb[FEEDBACK_LEVELS];
  
  ofstream fout;
  
  unsigned int totProcesses = 0;
  
  // Check If User Wants To Generate New Data And Generate It If So
  if (GENERATE == YES)
  {
    cout << argc << endl;
    if (argc == 3) // If there are 3 arguments, there is a new max/min bound
      generateNewData(fout, atoi(argv[1]), atoi(argv[2]));
    else
      generateNewData(fout);
  }   
  getProcesses(newProcesses, totProcesses);
  
  fout.open("output");
  
  // Start Threads That Will Run Different Scheduling Algorithms
  fifo = thread(firstInFirstOut, newProcesses, totProcesses, ref(lock), 
                ref(fout));
  spn = thread(shortestProcessNext, newProcesses, totProcesses, ref(lock), 
               ref(fout));
  srt = thread(shortestRemainingTime, newProcesses, totProcesses, ref(lock), 
               ref(fout));
  rr = thread(roundRobin, newProcesses, totProcesses, ref(lock), ref(fout));
  
  // Start All Feedback Threads At Different Levels
  for (unsigned int k = 0; k < FEEDBACK_LEVELS; k++)
    fb[k] = thread(feedback, newProcesses, totProcesses, ref(lock), 
                   k + FEEDBACK_OFFSET, ref(fout));
  
  // Join All Threads
  fifo.join();
  spn.join();
  srt.join();
  rr.join();
  
  // Join All Feedback Threads
  for (unsigned int k = 0; k < FEEDBACK_LEVELS; k++)
    fb[k].join();
  
  fout.close();
  
  return 0;
}
