//------------------------------------------------------------------------------
// Programmer: Charles Gaitley         Date: 11/22/2017
// Class: CS 3800                      Section: 1:00 PM
// File: ProcessAndScheduleFunctions.cpp
// Description: This File Contains All Function Implementations. This Includes
//              General And Process Functions.
//------------------------------------------------------------------------------

#include "Process.h"

//------------------------------------------------------------------------------
// General Function Implementations
//------------------------------------------------------------------------------

void generateNewData(ofstream &fout)
{
  const unsigned int MIN_DURATION = 1, MAX_DURATION = 100;
  generateNewData(fout, MIN_DURATION, MAX_DURATION); 
}


// This Function Will Generate New Data To Use In The Scheduling Analysis 
// Randomly
void generateNewData(ofstream &fout, const unsigned int lowerBound, const unsigned int upperBound)
{
  // Seed Random Number Generator
  srand(time(NULL));
  
  // Local Constants
  const unsigned int MIN_ARRIVAL_TIME = 1, MAX_ARRIVAL_TIME = 10000,
                     PROCESSES_TO_GENERATE = 1000;
  
  // Local Object
  vector<Process> generatedProcesses;
  
  // Generate PROCESSES_TO_GENERATE Amount Of Processes With A Random Arrival 
  // Time Between MIN_ARRIVAL_TIME And MAX_ARRIVAL_TIME And A Duration Between 
  // MIN_DURATION And MAX_DURATION
  for (unsigned int k = 0; k < PROCESSES_TO_GENERATE; k++)
  {
    Process p = Process(rand() % (MAX_ARRIVAL_TIME - MIN_ARRIVAL_TIME) + 
                        MIN_ARRIVAL_TIME, rand() % 
                       (upperBound - lowerBound) + lowerBound);
                       
    generatedProcesses.push_back(p);
  }
  
  sortProcesses(generatedProcesses, START, generatedProcesses.size() - OFFSET);
  
  fout.open("input");
  
  // Output All Random Data Into A File Called "input"
  for (auto it = generatedProcesses.begin(); it != generatedProcesses.end(); 
       it++)
    fout << (*it).getArrivalTime() << " " << (*it).getDurationOfProcess() 
         << endl;
  
  fout.close();
  
  return;
}

// This Function Will Sort All Processes In generatedProcesses By Their Arrival
// Times From Least To Greatest
void sortProcesses(vector<Process> &generatedProcesses, const unsigned int &low, 
                   const unsigned int &high)
{
  // Check If Partition Needed
  if (low < high)
  {
    unsigned int pivot = partition(generatedProcesses, low, high);
    
    sortProcesses(generatedProcesses, low, pivot);
    sortProcesses(generatedProcesses, pivot + OFFSET, high);
  }
  
  return;
}

// This Function Is Used To Partition generatedProcesses Properly And Help Sort
// It
unsigned int partition(vector<Process> &generatedProcesses, 
                       const unsigned int &low, const unsigned int &high)
{
  // Local Variable And Object
  Process piv = generatedProcesses[low];
  
  int i = low - OFFSET, j = high + OFFSET;
  
  // Find Processes That Need To Be Swapped In Partition
  while (true)
  {
    // Find Index Not Less Than Current Pivot
    do
    {
      i++;
    } while (generatedProcesses[i] < piv);
    
    // Find Index Not Greater Than Current Pivot
    do
    {
      j--;
    } while (generatedProcesses[j] > piv);
    
    // Return j As Pivot If This Is True
    if (i >= j)
      return j;
    
    swap(generatedProcesses, i, j);
  }
}

// This Function Will Swap Two Processes In An Array
void swap(vector<Process> &generatedProcesses, const unsigned int &index1, 
          const unsigned int &index2)
{
  Process temp = generatedProcesses[index1];
  generatedProcesses[index1] = generatedProcesses[index2];
  generatedProcesses[index2] = temp;
  
  return;
}

// This Function Will Load The newProcesses Queue With All Processes In The 
// "input" File
void getProcesses(deque<Process> &newProcesses, unsigned int &totProcesses)
{
  // Local Object And Variables
  ifstream fin;
  
  unsigned int arrival, duration;
  
  fin.open("input");
  
  // Read All Input From "input" File And Load It Into newProcesses
  while (fin >> arrival)
  {
    fin >> duration;
    Process p = Process(arrival, duration);
    newProcesses.push_back(p);
    totProcesses++;
  }
  
  fin.close();
  
  return;
}

// This Function Will Simulate The First-In First-Out Scheduling Algorithm
void firstInFirstOut(deque<Process> newProcesses, unsigned int totProcesses,
                     mutex &lock, ofstream &fout)
{
  // Local Objects And Variable
  deque<Process> readyProcesses, finishedProcesses;
  
  Process runningProcess;
  
  unsigned int simulationTime = 0;
  
  bool isRunning = false;
  
  // Loop While Processes Are Still Executing
  while (totProcesses)
  {
    simulationTime++;
    
    // Push Arriving Processes Into The Ready Queue
    while (simulationTime == newProcesses.front().getArrivalTime())
    {
      readyProcesses.push_back(newProcesses.front());
      newProcesses.pop_front();
    }
    
    // Check For No Running Processes And A Non-Empty Ready Queue And Start A
    // Process If Both Are True
    if (!isRunning && !readyProcesses.empty())
    {
      runningProcess = readyProcesses.front();
      readyProcesses.pop_front();
      isRunning = true;
    }
    
    // Check For A Running Process
    if (isRunning)
      // Check If Process Is Finished Running
      if (runningProcess.step())
      {
        runningProcess.setFinishTime(simulationTime);
        finishedProcesses.push_back(runningProcess);
        isRunning = false;
        totProcesses--;
      }
  }
  
  // Critical Section That Will Calculate Average Normalized Turn-Around Time
  // And Record Results In A File
  lock.lock();
  
  calcAveNormalTurnAroundTime(finishedProcesses, fout, FIRST_IN_FIRST_OUT);
  
  lock.unlock();
  
  return;
}

// This Function Will Simulate The Shortest Process Next Scheduling Algorithm
void shortestProcessNext(deque<Process> newProcesses, unsigned int totProcesses,
                         mutex &lock, ofstream &fout)
{
  // Local Objects And Variable
  deque<Process> readyProcesses, finishedProcesses;
  
  Process runningProcess;
  
  unsigned int simulationTime = 0;
  
  bool isRunning = false;
  
  // Loop While Processes Are Still Executing
  while (totProcesses)
  {
    simulationTime++;
    
    // Push Arriving Processes Into The Ready Queue
    while (simulationTime == newProcesses.front().getArrivalTime())
    {
      readyProcesses.push_back(newProcesses.front());
      newProcesses.pop_front();
    }
    
    // Check For No Running Process And Start The Shortest Process In Ready
    // Queue
    if (!isRunning && !readyProcesses.empty())
    {
      sortByRemaining(readyProcesses);
      runningProcess = readyProcesses.front();
      readyProcesses.pop_front();
      isRunning = true;
    }
    
    // Check For Running Process
    if (isRunning)
      // Run Process To Completion By One Time Unit
      if (runningProcess.step())
      {
        runningProcess.setFinishTime(simulationTime);
        finishedProcesses.push_back(runningProcess);
        isRunning = false;
        totProcesses--;
      }
  }
  
  // Critical Section That Will Calculate Average Normalized Turn-Around Time
  // And Record Results In A File
  lock.lock();
  
  calcAveNormalTurnAroundTime(finishedProcesses, fout, SHORTEST_TIME_NEXT);
  
  lock.unlock();
  
  return;
}

// This Function Will Simulate The Shortest Remaining Time Scheduling Algorithm
void shortestRemainingTime(deque<Process> newProcesses, 
                           unsigned int totProcesses, mutex &lock, 
                           ofstream &fout)
{
  // Local Objects And Variable
  deque<Process> readyProcesses, finishedProcesses;
  
  Process runningProcess;
  
  unsigned int simulationTime = 0;
  
  bool isRunning = false;
  
  // Loop While Processes Are Still Executing
  while (totProcesses)
  {
    simulationTime++;
    
    // Load Processes Into Ready Queue As They Arrive
    while (simulationTime == newProcesses.front().getArrivalTime())
    {
      readyProcesses.push_back(newProcesses.front());
      newProcesses.pop_front();
    }
    
    // Check For No Running Process And Start The Shortest One If So
    if (!isRunning && !readyProcesses.empty())
    {
      sortByRemaining(readyProcesses);
      runningProcess = readyProcesses.front();
      readyProcesses.pop_front();
      isRunning = true;
    }
    // Check If Process Needs To Be Pre-empted
    else if (isRunning && !readyProcesses.empty())
    {
      sortByRemaining(readyProcesses);
      
      // Check For Pre-emption
      if (readyProcesses.front().getRemainingTime() < 
          runningProcess.getRemainingTime())
      {
        readyProcesses.push_back(runningProcess);
        runningProcess = readyProcesses.front();
        readyProcesses.pop_front();
      }
    }
    
    // Check For Running Process
    if (isRunning)
      // Run Process To Completion By One Time Unit
      if (runningProcess.step())
      {
        runningProcess.setFinishTime(simulationTime);
        finishedProcesses.push_back(runningProcess);
        isRunning = false;
        totProcesses--;
      }
  }
  
  // Critical Section That Will Calculate Average Normalized Turn-Around Time
  // And Record Results In A File
  lock.lock();
  
  calcAveNormalTurnAroundTime(finishedProcesses, fout, SHORTEST_REMAINING_TIME);
  
  lock.unlock();
  
  return;
}

// This Function Will Simulate The Round Robin Scheduling Algorithm
void roundRobin(deque<Process> newProcesses, unsigned int totProcesses,
                mutex &lock, ofstream &fout)
{
  // Local Objects And Variable
  deque<Process> readyProcesses, finishedProcesses;
  
  Process runningProcess;
  
  unsigned int simulationTime = 0, quanTime, 
               timeQuantum = calcTimeQuantum(newProcesses);
  
  bool isRunning = false;
  
  // Loop While Processes Are Still Executing
  while (totProcesses)
  {
    simulationTime++;
    
    // Push New Processes To The Ready Queue
    while (simulationTime == newProcesses.front().getArrivalTime())
    {
      readyProcesses.push_back(newProcesses.front());
      newProcesses.pop_front();
    }
    
    // Start A Ready Process If No Other Processes Running And There Is One 
    // Ready
    if (!isRunning && !readyProcesses.empty())
    {
      runningProcess = readyProcesses.front();
      readyProcesses.pop_front();
      quanTime = timeQuantum;
      isRunning = true;
    }
    
    // Check For Running Process
    if (isRunning)
    {
      quanTime--;
      
      // Check For Process Done Executing
      if (runningProcess.step())
      {
        runningProcess.setFinishTime(simulationTime);
        finishedProcesses.push_back(runningProcess);
        isRunning = false;
        totProcesses--;
      }
      // Check For Time Quantum Reached
      else if (quanTime == QUANTUM_FINISHED)
      {
        readyProcesses.push_back(runningProcess);
        isRunning = false;
      }
    }
  }
  
  // Critical Section That Will Calculate Average Normalized Turn-Around Time
  // And Record Results In A File
  lock.lock();
  
  calcAveNormalTurnAroundTime(finishedProcesses, fout, ROUND_ROBIN);
  
  lock.unlock();
  
  return;
}

// This Function Will Simulate The Feedback Scheduling Algorithm With Min Level
// Of Two And Max Level Of Five
void feedback(deque<Process> newProcesses, unsigned int totProcesses,
              mutex &lock, const unsigned int &feedbackLevel, ofstream &fout)
{
  // Local Constants
  const int STOP = -1;
  
  const unsigned int DIVISOR = 2, MIN_QUANTUM = 2, FIRST_QUANTUM = 0, 
                     SECOND_QUANTUM = 1, THIRD_QUANTUM = 2, FOURTH_QUANTUM = 3,
                     FIFTH_QUANTUM = 4;
  
  // Local Objects And Variable
  deque<Process> top, midLevel1, midLevel2, midLevel3, base, finishedProcesses;
  
  Process runningProcess;
  
  unsigned int simulationTime = 0, *quantums, quanTime;
  
  bool isRunning = false, first = false, second = false, third = false, 
       fourth = false, fifth = false;
  
  quantums = new unsigned int[feedbackLevel];
  
  // Assign Time Quantums To Each Queue
  for (int k = feedbackLevel - OFFSET; k > STOP; k--)
  {
    // Check For First Iteration And Set Base Time Quantum
    if (k == feedbackLevel - OFFSET)
      quantums[k] = calcTimeQuantum(newProcesses);
    else
      quantums[k] = quantums[k + OFFSET] / DIVISOR;
      
    // Check For Invalid Quantum And Correct
    if (quantums[k] < MIN_QUANTUM)
      quantums[k] = MIN_QUANTUM;
  }
  
  // Correct Duplicate Quantums
  for (unsigned int k = feedbackLevel - OFFSET; k > STOP + OFFSET; k--)
  {
    // For-Loop Variable
    unsigned int count = 0;
    
    // Find Elements Same As Current Element 
    for (int j = k - OFFSET; j > STOP; j--)
      // Check For Equality Of Elements
      if (quantums[k] == quantums[j])
        count++;
    
    quantums[k] += count; 
  }
  
  // Loop While Processes Are Still Executing
  while (totProcesses)
  {
    simulationTime++;
    
    // Push New Processes Into The Top-Level Queue
    while (simulationTime == newProcesses.front().getArrivalTime())
    {
      top.push_back(newProcesses.front());
      newProcesses.pop_front();
    }
    
    // Select Which Queue To Get Process To Run From And Appropriate Time 
    // Quantum
    if (!isRunning && !top.empty())
    {
      runningProcess = top.front();
      top.pop_front();
      quanTime = quantums[FIRST_QUANTUM];
      first = true;
      isRunning = true;
    }
    // Second Queue Selected
    else if (feedbackLevel > FEEDBACK_LEVEL_2 && !isRunning && 
             !midLevel1.empty())
    {
      runningProcess = midLevel1.front();
      midLevel1.pop_front();
      quanTime = quantums[SECOND_QUANTUM];
      second = true;
      isRunning = true;
    }
    // Third Queue Selected
    else if (feedbackLevel > FEEDBACK_LEVEL_3 && !isRunning && 
             !midLevel2.empty())
    {
      runningProcess = midLevel2.front();
      midLevel2.pop_front();
      quanTime = quantums[THIRD_QUANTUM];
      third = true;
      isRunning = true;
    }
    // Fourth Queue Selected
    else if (feedbackLevel > FEEDBACK_LEVEL_4 && !isRunning && 
             !midLevel3.empty())
    {
      runningProcess = midLevel3.front();
      midLevel3.pop_front();
      quanTime = quantums[FOURTH_QUANTUM];
      fourth = true;
      isRunning = true;
    }
    // Fifth Queue Selected
    else if (!isRunning && !base.empty())
    {
      runningProcess = base.front();
      base.pop_front();
      quanTime = quantums[FIFTH_QUANTUM];
      fifth = true;
      isRunning = true;
    }
    
    // Check For Running Process
    if (isRunning)
    {
      quanTime--;
      
      // Check For Process Finished
      if (runningProcess.step())
      {
        runningProcess.setFinishTime(simulationTime);
        finishedProcesses.push_back(runningProcess);
        isRunning = false;
        first = false;
        second = false;
        third = false;
        fourth = false;
        fifth = false;
        totProcesses--;
      }
      // Check For Time Quantum Reached
      else if (quanTime == QUANTUM_FINISHED)
      {
        // Check For Which Queue To Push Into Next
        if (first && feedbackLevel > FEEDBACK_LEVEL_2)
        {
          midLevel1.push_back(runningProcess);
          first = false;
        }
        // Second Queue With Higher Level
        else if (second && feedbackLevel > FEEDBACK_LEVEL_3)
        {
          midLevel2.push_back(runningProcess);
          second = false;
        }
        // Third Queue With Higher Level
        else if (third && feedbackLevel > FEEDBACK_LEVEL_4)
        {
          midLevel3.push_back(runningProcess);
          third = false;
        }
        // First Queue With No Higher Level
        else if (first)
        {
          base.push_back(runningProcess);
          first = false;
        }
        // Second Queue With No Higher Level
        else if (second)
        {
          base.push_back(runningProcess);
          second = false;
        }
        // Third Queue With No Higher Level
        else if (third)
        {
          base.push_back(runningProcess);
          third = false;
        }
        // Fourth Queue
        else if (fourth)
        {
          base.push_back(runningProcess);
          fourth = false;
        }
        // Fifth Queue
        else if (fifth)
        {
          base.push_back(runningProcess);
          fifth = false;
        }
        
        isRunning = false;
      }
    }
  }
  
  delete [] quantums;
  
  // Critical Section That Will Calculate Average Normalized Turn-Around Time
  // And Record Results In A File
  lock.lock();
  
  calcAveNormalTurnAroundTime(finishedProcesses, fout, feedbackLevel);
  
  lock.unlock();
  
  return;
}

// This Function Will Calculate The Average Normalized Turn-Around Time Of Some
// Processes And Output It To A File Called "output"
void calcAveNormalTurnAroundTime(const deque<Process> &finishedProcesses,
                                 ofstream &fout, const unsigned int &algorithm)
{
  // Local Variable
  unsigned int aveNormalTurnAroundTime = 0;
  
  // Go Through finishedProcesses Queue And Sum All Normalized Turn-Around Times
  for (auto it = finishedProcesses.begin(); it != finishedProcesses.end(); it++)
    aveNormalTurnAroundTime += (*it).getNormalizedTurnAroundTime();
  
  // Output Beginning Based On Algoithm Which Called This Function
  switch (algorithm)
  {
    case FIRST_IN_FIRST_OUT:
      fout << "First In First Out\nFIFO\n";
      
      break;
    case FEEDBACK_LEVEL_2:
      fout << "Feedback Level 2\nFB2\n";
      
      break;
    case FEEDBACK_LEVEL_3:
      fout << "Feedback Level 3\nFB3\n";
      
      break;
    case FEEDBACK_LEVEL_4:
      fout << "Feedback Level 4\nFB4\n";
      
      break;
    case FEEDBACK_LEVEL_5:
      fout << "Feedback Level 5\nFB5\n";
      
      break;
    case SHORTEST_TIME_NEXT:
      fout << "Shortest Time Next\nSTN\n";
      
      break;
    case SHORTEST_REMAINING_TIME:
      fout << "Shortest Remaining Time\nSRT\n";
      
      break;
    case ROUND_ROBIN:
      fout << "Round Robin\nRR\n";
      
      break;
    default:
      cout << "Something went really wrong.  Exiting..." << endl;
      
      exit(EXIT_FAILURE);
      
      break;
  }
  
  fout << static_cast<double>(aveNormalTurnAroundTime) / 
          finishedProcesses.size() << endl;
  
  return;
}

// This Function Will Calculate A Time Quantum For The Set Of All New Processes
unsigned int calcTimeQuantum(const deque<Process> &newProcesses)
{
  // Local Variable
  unsigned int aveTime = 0;
  
  // Sum Up All Process Durations
  for (auto it = newProcesses.begin(); it != newProcesses.end(); it++)
    aveTime += (*it).getDurationOfProcess();
  
  return (aveTime / newProcesses.size());
}

// This Function Will Sort The Ready Processes By Their Remaining Time From 
// Least To Greatest
void sortByRemaining(deque<Process> &readyProcesses)
{
  // Local Object
  vector<Process> ready;
  
  // Convert Queue To A Vector
  while (!readyProcesses.empty())
  {
    ready.push_back(readyProcesses.front());
    readyProcesses.pop_front();
  }
  
  // Sort By Remaining Time
  for (unsigned int k = 0; k < ready.size() - OFFSET; k++)
  {
    unsigned int min = k;
    
    // Check For New Mininmum
    for (unsigned int j = k + OFFSET; j < ready.size(); j++)
      if (ready[j].getRemainingTime() < ready[min].getRemainingTime())
        min = j;
    
    swap(ready, k, min);
  }
  
  // Convert Vector Back To Queue
  for (auto it = ready.begin(); it != ready.end(); it++)
    readyProcesses.push_back(*it);
  
  return;
}

//------------------------------------------------------------------------------
// Process Function Implementations
//------------------------------------------------------------------------------

// Parametrized Constructor To Specify Duration And Arrival Of A Process
Process::Process(const unsigned int &arrival, const unsigned int &duration)
{
  arr = arrival;
  dur = duration;
  rem = duration;
}

// This Function Will Simulate One Time Unit Of Process Execution
inline bool Process::step()
{
  rem--;
  
  return finished();
}

// This Function Will Check If A Process Is Finished
inline bool Process::finished()
{
  return (rem <= MIN_REMAINING);
}

// This Function Will Return The Remaining Time Of This Process
inline unsigned int Process::getRemainingTime() const
{
  return rem;
}

// This Function Will Return The Arrival Time Of This Process
inline unsigned int Process::getArrivalTime() const
{
  return arr;
}

// This Function Will Return The Duration Of This Process
inline unsigned int Process::getDurationOfProcess() const
{
  return dur;
}

// This Function Will Return The Finish Time Of This Process
inline unsigned int Process::getFinishTime() const
{
  return fin;
}

// This Function Will Change The Remaining Time Of This Process To A New Time
inline void Process::setRemainingTime(const unsigned int &remTime)
{
  rem = remTime;
  
  return;
}

// This Function Will Change The Arrival Time Of This Process To A New Time
inline void Process::setArrivalTime(const unsigned int &arrTime)
{
  arr = arrTime;
  
  return;
}

// This Function Will Change The Duration Of This Process To A New Time
inline void Process::setDurationOfProcess(const unsigned int &durTime)
{
  dur = durTime;
  
  return;
}

// This Function Will Change The Finish Time Of This Process To A New Time
inline void Process::setFinishTime(const unsigned int &finTime)
{
  fin = finTime;
  
  return;
}

// This Function Will Return The Turn-Around Time Of This Process
inline unsigned int Process::getTurnAroundTime() const
{
  return (fin - arr);
}

// This Function Will Return The Normalized Turn-Around Time Of This Process
inline double Process::getNormalizedTurnAroundTime() const
{
  // Local Constant
  const double NORMAL_OFFSET = 1.0;
  
  return (static_cast<double>(fin - arr + NORMAL_OFFSET) / dur);
}

//------------------------------------------------------------------------------
// Friend Of The Process Class Functions
//------------------------------------------------------------------------------

// This Function Will Compare Two Processes On A Strictly Less-Than Basis
bool operator < (const Process &lhs, const Process &rhs)
{
  return (lhs.arr < rhs.arr);
}

// This Function Will Compare Two Processes On A Strictly Greater-Than Basis
bool operator > (const Process &lhs, const Process &rhs)
{
  return (lhs.arr > rhs.arr);
}
