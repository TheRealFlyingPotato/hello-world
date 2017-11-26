//------------------------------------------------------------------------------
// Programmer: Charles Gaitley         Date: 11/22/2017
// Class: CS 3800                      Section: 1:00 PM
// File: Process.h
// Description: This File Contains Any Need Libraries, Namespaces, Global 
//              Constants, And Function Prototypes That Will Be Used In 
//              Executable Files.  This File Will Also Contain The Process Class 
//              (Very Similar To simProcess Class).
//------------------------------------------------------------------------------

// Avoid Redefinition Errors
#ifndef PROCESS_H
#define PROCESS_H

//------------------------------------------------------------------------------
// Libraries
//------------------------------------------------------------------------------

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <string>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <vector>

//------------------------------------------------------------------------------
// Namespace
//------------------------------------------------------------------------------

using namespace std;

//------------------------------------------------------------------------------
// Global Constants
//------------------------------------------------------------------------------

const unsigned int MIN_ARRIVAL = 0, MIN_REMAINING = 0, MIN_FINISH = 0, 
                   MIN_DURATION = 0, OFFSET = 1, ROUND_ROBIN = 8, 
                   SHORTEST_REMAINING_TIME = 7, SHORTEST_TIME_NEXT = 6,
                   FEEDBACK_LEVEL_5 = 5, FEEDBACK_LEVEL_4 = 4, 
                   FEEDBACK_LEVEL_3 = 3, FEEDBACK_LEVEL_2 = 2, 
                   FIRST_IN_FIRST_OUT = 1, QUANTUM_FINISHED = 0, START = 0;

//------------------------------------------------------------------------------
// Class Prototype
//------------------------------------------------------------------------------

// Represents A Process In An Operating System
class Process;

//------------------------------------------------------------------------------
// General Function Prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Description   : This Function Will Generate New Data To Use In The Scheduling
//                 Analysis Randomly
// Pre           : User Has Indicated That New Data Should Be Generated
// Post          : New Data Has Been Randomly Generated And Is Ready For Use In
//                 File Named "input"
//------------------------------------------------------------------------------
void generateNewData(ofstream &fout, const unsigned int lowerBound, const unsigned int upperBound);
void generateNewData(ofstream &fout);

//------------------------------------------------------------------------------
// Description   : This Function Will Sort All Processes In generatedProcesses 
//                 By Their Arrival Times From Least To Greatest
// Pre           : generatedProcesses Is Filled With PROCESSES_TO_GENERATE 
//                 Amount Of Processes
// Post          : generatedProcesses Is Sort From Least To Greatest By Arrival
//                 Time Of Each Process In It
//------------------------------------------------------------------------------
void sortProcesses(vector<Process> &generatedProcesses, const unsigned int &low, 
                   const unsigned int &high);

//------------------------------------------------------------------------------
// Description   : This Function Is Used To Partition generatedProcesses 
//                 Properly And Help Sort It
// Pre           : generatedProcesses Is Filled With PROCESSES_TO_GENERATE
//                 Amount Of Processes
// Post          : A Pivot Is Returned To Be Sorted Next
//------------------------------------------------------------------------------
unsigned int partition(vector<Process> &generatedProcesses, 
                       const unsigned int &low, const unsigned int &high);

//------------------------------------------------------------------------------
// Description   : This Function Will Swap Two Processes In An Array
// Pre           : index1 And index2 In generatedProcesses
// Post          : Process At index1 Is Now At index2 And Process At index2 Is
//                 Now At index1
//------------------------------------------------------------------------------
void swap(vector<Process> &generatedProcesses, const unsigned int &index1, 
          const unsigned int &index2);

//------------------------------------------------------------------------------
// Description   : This Function Will Load The newProcesses Queue With All 
//                 Processes In The "input" File
// Pre           : None
// Post          : newProcesses Is Now Load With All Processes From The "input"
//                 File
//------------------------------------------------------------------------------
void getProcesses(deque<Process> &newProcesses, unsigned int &totProcesses);

//------------------------------------------------------------------------------
// Description   : This Function Will Simulate The First-In First-Out Scheduling
//                 Algorithm
// Pre           : newProcesses Has PROCESSES_TO_GENERATE Number Of Processes 
//                 And totProcesses = PROCESSES_TO_GENERATE (See generateNewData
//                 Function)
// Post          : First-In First-Out Scheduling Algorithm Successfully 
//                 Simulated
//------------------------------------------------------------------------------
void firstInFirstOut(deque<Process> newProcesses, unsigned int totProcesses,
                     mutex &lock, ofstream &fout);

//------------------------------------------------------------------------------
// Description   : This Function Will Simulate The Shortest Process Next 
//                 Scheduling Algorithm
// Pre           : newProcesses Has PROCESSES_TO_GENERATE Number Of Processes 
//                 And totProcesses = PROCESSES_TO_GENERATE (See generateNewData
//                 Function)
// Post          : Shortest Process Next Scheduling Algorithm Successfully 
//                 Simulated
//------------------------------------------------------------------------------
void shortestProcessNext(deque<Process> newProcesses, unsigned int totProcesses,
                         mutex &lock, ofstream &fout);

//------------------------------------------------------------------------------
// Description   : This Function Will Simulate The Shortest Remaining Time 
//                 Scheduling Algorithm
// Pre           : newProcesses Has PROCESSES_TO_GENERATE Number Of Processes 
//                 And totProcesses = PROCESSES_TO_GENERATE (See generateNewData
//                 Function)
// Post          : Shortest Remaing Time Scheduling Algorithm Successfully 
//                 Simulated
//------------------------------------------------------------------------------
void shortestRemainingTime(deque<Process> newProcesses, 
                           unsigned int totProcesses, mutex &lock, 
                           ofstream &fout);

//------------------------------------------------------------------------------
// Description   : This Function Will Simulate The Round Robin Scheduling 
//                 Algorithm
// Pre           : newProcesses Has PROCESSES_TO_GENERATE Number Of Processes 
//                 And totProcesses = PROCESSES_TO_GENERATE (See generateNewData
//                 Function)
// Post          : Round Robin Scheduling Algorithm Successfully Simulated
//------------------------------------------------------------------------------
void roundRobin(deque<Process> newProcesses, unsigned int totProcesses, 
                mutex &lock, ofstream &fout);

//------------------------------------------------------------------------------
// Description   : This Function Will Simulate The Feedback Scheduling Algorithm
//                 With Min Level Of Two And Max Level Of Five
// Pre           : newProcesses Has PROCESSES_TO_GENERATE Number Of Processes 
//                 And totProcesses = PROCESSES_TO_GENERATE (See generateNewData
//                 Function)
// Post          : Feedback Scheduling Algorithm Successfully Simulated
//------------------------------------------------------------------------------
void feedback(deque<Process> newProcesses, unsigned int totProcesses, 
              mutex &lock, const unsigned int &feedbackLevel, ofstream &fout);

//------------------------------------------------------------------------------
// Description   : This Function Will Calculate The Average Normalized 
//                 Turn-Around Time Of Some Processes And Output It To A File
//                 Called "output"
// Pre           : finishedProcesses Is Of Size PROCESSES_TO_GENERATE (See 
//                 generateNewData Function)
// Post          : The Average Normalized Time Has Been Written To A File Called
//                 "output"
//------------------------------------------------------------------------------
void calcAveNormalTurnAroundTime(const deque<Process> &finishedProcesses,
                                 ofstream &fout, const unsigned int &algorithm);

//------------------------------------------------------------------------------
// Description   : This Function Will Calculate A Time Quantum For The Set Of 
//                 All New Processes
// Pre           : newProcesses Is Of Size PROCESSES_TO_GENERATE (See 
//                 generateNewData Function)
// Post          : The Average Duration Of Each Process Is Returned Truncated
//------------------------------------------------------------------------------
unsigned int calcTimeQuantum(const deque<Process> &newProcesses);

//------------------------------------------------------------------------------
// Description   : This Function Will Sort The Ready Processes By Their 
//                 Remaining Time From Least To Greatest
// Pre           : readyProcesses Queue Is Not Empty
// Post          : The readyProcesses Queue Has Been Sorted By Remianing Time
//                 From Least To Greatest
//------------------------------------------------------------------------------
void sortByRemaining(deque<Process> &readyProcesses);                                 

//------------------------------------------------------------------------------
// Class Definition
//------------------------------------------------------------------------------

// This Class Represents A Process In An Operating System
class Process
{
  
  //----------------------------------------------------------------------------
  // Private Data Members
  //----------------------------------------------------------------------------
  
  private:
    unsigned int arr, dur, rem, fin; // Arrival, Duration, Remaining Time, And
                                     // Finish Time Of This Process
  
  //----------------------------------------------------------------------------
  // Public Process Function Prototypes
  //----------------------------------------------------------------------------
  
  public:
    
    //--------------------------------------------------------------------------
    // Description   : Default Constructor For Process Class
    // Pre           : None
    // Post          : All Private Data Members Defaulted To Zero
    // --INLINE
    //--------------------------------------------------------------------------
    Process(): arr(MIN_ARRIVAL), dur(MIN_DURATION), rem(MIN_REMAINING), 
               fin(MIN_FINISH) {}
    
    //--------------------------------------------------------------------------
    // Description   : Parametrized Constructor To Specify Duration And Arrival 
    //                 Of A Process
    // Pre           : Two Positive Integers To Represent An Arrival Time And 
    //                 Duration
    // Post          : arr, dur, And rem Set But fin Is Yet To Be Set
    //--------------------------------------------------------------------------
    Process(const unsigned int &arrival, const unsigned int &duration);
    
    //--------------------------------------------------------------------------
    // Description   : This Function Will Simulate One Time Unit Of Process 
    //                 Execution
    // Pre           : None
    // Post          : This Process Has Been Executed For One Time Unit
    //--------------------------------------------------------------------------
    inline bool step();
    
    //--------------------------------------------------------------------------
    // Description   : This Function Will Check If A Process Is Finished
    // Pre           : None
    // Post          : A bool Is Returned. True = Finisihed; False = Not 
    //                 Finished
    //--------------------------------------------------------------------------
    inline bool finished();
    
    //--------------------------------------------------------------------------
    // Description   : This Function Will Return The Remaining Time Of This 
    //                 Process
    // Pre           : None
    // Post          : The Remaining Time Of This Process Is Returned
    //--------------------------------------------------------------------------
    inline unsigned int getRemainingTime() const;
    
    //--------------------------------------------------------------------------
    // Description   : This Function Will Return The Arrival Time Of This 
    //                 Process
    // Pre           : None
    // Post          : The Arrival Time Of This Process Is Returned
    //--------------------------------------------------------------------------
    inline unsigned int getArrivalTime() const;
    
    //--------------------------------------------------------------------------
    // Description   : This Function Will Return The Duration Of This Process
    // Pre           : None
    // Post          : The Duration Of This Process Is Returned
    //--------------------------------------------------------------------------
    inline unsigned int getDurationOfProcess() const;
    
    //--------------------------------------------------------------------------
    // Description   : This Function Will Return The Finish Time Of This Process
    // Pre           : None
    // Post          : The Finish Time Of This Process Is Returned
    //--------------------------------------------------------------------------
    inline unsigned int getFinishTime() const;
    
    //--------------------------------------------------------------------------
    // Description   : This Function Will Change The Remaining Time Of This 
    //                 Process To A New Time 
    // Pre           : remTime >= 0
    // Post          : The Remaining Time Of This Process Now Has The Value Of
    //                 remTime
    //--------------------------------------------------------------------------
    inline void setRemainingTime(const unsigned int &remTime);
    
    //--------------------------------------------------------------------------
    // Description   : This Function Will Change The Arrival Time Of This Process 
    //                 To A New Time 
    // Pre           : arrTime >= 0
    // Post          : The Arrival Time Of This Process Now Has The Value Of
    //                 arrTime
    //--------------------------------------------------------------------------
    inline void setArrivalTime(const unsigned int &arrTime);
    
    //--------------------------------------------------------------------------
    // Description   : This Function Will Change The Duration Of This Process To 
    //                 A New Duration
    // Pre           : durTime >= 0
    // Post          : The Duration Of This Process Now Has The Value Of durTime
    //--------------------------------------------------------------------------
    inline void setDurationOfProcess(const unsigned int &durTime);
    
    //--------------------------------------------------------------------------
    // Description   : This Function Will Change The Finish Time Of This Process 
    //                 To A New Time 
    // Pre           : finTime >= 0
    // Post          : The Finsih Time Of This Process Now Has The Value Of
    //                 finTime
    //--------------------------------------------------------------------------
    inline void setFinishTime(const unsigned int &finTime);
    
    //--------------------------------------------------------------------------
    // Description   : This Function Will Return The Turn-Around Time Of This
    //                 Process
    // Pre           : fin >= arr
    // Post          : The Turn-Around Time Of This Process Is Returned
    //--------------------------------------------------------------------------
    inline unsigned int getTurnAroundTime() const;
    
    //--------------------------------------------------------------------------
    // Description   : This Function Will Return The Normalized Turn-Around Time 
    //                 Of This Process
    // Pre           : fin >= arr; dur != 0
    // Post          : The Normalized Turn-Around Time Of This Process Is 
    //                 Returned
    //--------------------------------------------------------------------------
    inline double getNormalizedTurnAroundTime() const;
    
    //--------------------------------------------------------------------------
    // Description   : This Function Will Compare Two Processes On A Strictly
    //                 Less-Than Basis
    // Pre           : lhs.arr >= 0 And rhs.arr >= 0
    // Post          : True Is Returned If lhs Has A Lesser Arrival Time Than
    //                 rhs. False Otherwise
    //--------------------------------------------------------------------------
    friend bool operator < (const Process &lhs, const Process &rhs);
    
    //--------------------------------------------------------------------------
    // Description   : This Function Will Compare Two Processes On A Strictly
    //                 Greater-Than Basis
    // Pre           : lhs.arr >= 0 And rhs.arr >= 0
    // Post          : True Is Returned If lhs Has A Greater Arrival Time Than
    //                 rhs. False Otherwise
    //--------------------------------------------------------------------------
    friend bool operator > (const Process &lhs, const Process &rhs);
};

#endif
