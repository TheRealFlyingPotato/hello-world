#include <iostream>
#include <fstream>
#include <queue>
#include "simProcess.h"

using namespace std;

enum {
  S_FIFO,
  S_RR,
  S_FB
};

enum {
  TOP,
  MID,
  BASE
};

//select an algorithm for scheduling
const int ALGORITHM = S_FIFO;

const int TIME_QUANTUM = 2;
const int MID_QUANTUM = 3;
const int BASE_QUANTUM = 4;
const int FEEDBACK_LEVEL = 2;

int main() {
  //setup queues for processes (based on state)
  deque<simProcess> newProcs;
  deque<simProcess> readyProcs;
  deque<simProcess> exitProcs;
  deque<simProcess> topProcs;
  deque<simProcess> midProcs;
  deque<simProcess> baseProcs;

  simProcess runningProc; //can only be one process running at a time
  bool isRunning = false;

  int procSource;
  int currentQuantum = 0;
  //load simulation data
  unsigned int arrival, duration;
  
  ifstream fin;
  fin.open("input");

  int toProcess = 0;
  while (fin >> arrival) {
    toProcess++;
    fin >> duration;
    simProcess sp = simProcess(arrival, duration);
    newProcs.push_back(sp);
  }

  fin.close();

  //////////////////////////////ready for simulation
  //continue until there are no new or ready processes (everything has finished running)
  unsigned int simTime = 0; //cap at 999999 to prevent infinite loop
  while (toProcess && /*!(newProcs.empty() && readyProcs.empty()) &&*/ (simTime < 999999)) {
    switch(ALGORITHM) { 
    case S_FIFO:
      simTime++; //update simulation time
//cout << "Sim time now " << simTime << endl;
      //any arriving processes (per simulation time) can go into the ready queue
      while (simTime == newProcs.front().getArrival()) {
        readyProcs.push_back(newProcs.front());
        newProcs.pop_front();
//cout << "Popped from newProcs " << readyProcs.back().getArrival() << endl;
      }
      if (!isRunning && !readyProcs.empty()) { //if no process WAS running, BUT we have one
        runningProc = readyProcs.front(); //pull first available process
        readyProcs.pop_front(); //no longer "ready" - it's "running"
        isRunning = true;
//cout << "Running process " << runningProc.getArrival() << endl;
      }
      if (isRunning) { //"process" one work unit
        if (runningProc.process()) { //if job has completed, move to finished queue
          runningProc.setFinished(simTime+1); //note finished time for calculations
          exitProcs.push_back(runningProc); //switch queues
          isRunning = false; //prepare for the next process
          toProcess--;
/*cout << "Finished process normalized Turn-around time " 
     << exitProcs.back().getNormalizedTurnaround() << endl
     << exitProcs.back().getArrival() << endl
     << exitProcs.back().getFinish() << endl
     << exitProcs.back().getDuration() << endl;
*/     
        }
//cout << runningProc.getRemaining() << " work left on running process" << endl;
      }
    break;
    case S_RR:
      simTime++; // update simulation time
      while (simTime == newProcs.front().getArrival()) {
        readyProcs.push_back(newProcs.front());
        newProcs.pop_front();
//cout << "Popped from newProcs " << readyProcs.back().getArrival() << endl;
      }
       
      if (!isRunning && !readyProcs.empty()) { //if no process WAS running, BUT we have one
        runningProc = readyProcs.front(); //pull first available process
        readyProcs.pop_front(); //no longer "ready" - it's "running"
        isRunning = true;
	currentQuantum = 0;
//cout << "Running process " << runningProc.getArrival() << endl;
      }
      if (isRunning) {
        currentQuantum++;
        if (runningProc.process()) {
          runningProc.setFinished(simTime+1);
          exitProcs.push_back(runningProc);
          isRunning = false;
          toProcess--;
        }
        else if (currentQuantum == TIME_QUANTUM && !readyProcs.empty()) {
          readyProcs.push_back(runningProc);
          isRunning = false;
        }
      }
    break;
    case S_FB:
      simTime++;
          while (simTime == newProcs.front().getArrival()) {
            readyProcs.push_back(newProcs.front());
            newProcs.pop_front();
//cout << "Popped from newProcs " << readyProcs.back().getArrival() << endl;
          }
          if (!isRunning && !readyProcs.empty()) {
            runningProc = readyProcs.front();
   	    readyProcs.pop_front(); // pull first availible
	    isRunning = true;
            currentQuantum = TIME_QUANTUM;
            procSource = TOP;
          }
	  else if (FEEDBACK_LEVEL == 3 && !isRunning && !midProcs.empty()) {
	    runningProc = midProcs.front();
	    midProcs.pop_front();
	    isRunning = true;
	    currentQuantum = MID_QUANTUM;
	    procSource = MID;
	  }
          else if (!isRunning && !baseProcs.empty()) {
            runningProc = baseProcs.front();
            baseProcs.pop_front();
            isRunning = true;
            currentQuantum = BASE_QUANTUM;
 	    procSource = BASE;
          }

	  if (isRunning) {
            currentQuantum--;
	    if (runningProc.process()) {
		runningProc.setFinished(simTime+1);
		exitProcs.push_back(runningProc);
		isRunning = false;
          	toProcess--;
	    }
	    else if (currentQuantum == 0) {
		if (procSource != TOP || FEEDBACK_LEVEL == 2) 
		  baseProcs.push_back(runningProc);
		else
		  midProcs.push_back(runningProc);
		isRunning = false;
	    }
          }
    break;
    ///////////////////////////////////////////////////////////////////
    ///////////Intentionally no default block - no scheduling alg == error
    default:
      cout << "ERROR - need a scheduling algorithm!" << endl << endl;
    }

  }
   
  cout << "There are " << exitProcs.size() << " finished processes" << endl;
  unsigned int avgNTaT = 0;
  for (deque<simProcess>::iterator it = exitProcs.begin(); it != exitProcs.end(); it++) {
    avgNTaT += (*it).getNormalizedTurnaround();
  }
  cout << " with an average normalized turnaround time of " 
       << static_cast<double>(avgNTaT) / exitProcs.size() << endl;

  return 0;
}
