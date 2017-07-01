#include <string>                             // std::basic_string class template
#include <map>                                // std::map and std::multimap associative containers
#include <queue>                              // std::queue and std::priority_queue container adaptors
#include <iostream>                           // several standard stream objects
#include <fstream>                            // std::basic_fstream, std::basic_ifstream, std::basic_ofstream class templates and several typedefs
#include <sstream>                            // std::basic_stringstream, std::basic_istringstream, std::basic_ostringstream class templates and several typedefs

using namespace std;

// global variables
int clockTime = 0;
int nCores = 0;
int coresAvailable = 0;
bool diskAvailable = true;
int processesInMemory = 0;
int pushTime = 0;

// clock time
// number of cores
// cores available
// disk is available
// processes are in memory

struct event
{
     string operation = "";
     int timeValue = 0;
};

struct process
{
     int id = 0;
     int startTime = 0;
     int stopTime = 0;
     string status = "CREATING";

     queue<event> nextEvents;

     void startEvent()
     {
          nextTime = clockTime + nextEvents.front().timeValue;

          if ( nextEvents.front().operation == "CORE" || nextEvents.front().operation == "DISK" )
          {
               status = "RUNNING";
          }
          if ( nextEvents.front().operation == "DISPLAY" || nextEvents.front().operation == "INPUT" )
          {
               status = "BLOCKED";
          }
     }

     void finishEvent()
     {
          nextEvents.pop();
     }

     int nextTime = 0;
     int pushTime = 0;
     struct compareTIME
     {
          bool operator()(process const & a, process const & b)
          {
               if ( a.nextTime > b.nextTime )
               {
                    return true;
               }
               //if ( a.nextTime == b.nextTime )
               //{
               //     if ( a.pushTime > b.pushTime )
               //     {
               //          return true;
               //     }
               //}
               return false;
          }
     };
};

priority_queue<process, vector<process>, process::compareTIME> waiting;
priority_queue<process, vector<process>, process::compareTIME> running;

map<int, process> processTable;
void updateTable(process & p)
{
     map<int, process>::iterator it = processTable.find(p.id);
     if ( it != processTable.end() )
     {
          it->second = p;
     }
}
void preemptProcess(process p)
{
     // Can't start the process.
     // Set its nextTime to the nextTime of the top running process.
     // Note: A process can only be preempted if another process is running.
     //       Therefore, we don't have to check if running is empty.

     queue<process> temp;
     // Check running queue for next process with equal operation
     while ( !running.empty() )
     {
          process p2 = running.top();
          if ( p.nextEvents.front().operation == p2.nextEvents.front().operation )
          {
               p.nextTime = running.top().nextTime;
               break;
          }
          temp.push(p2);
          running.pop();
     }
     // Push processes back onto running queue.
     while ( !temp.empty() )
     {
          running.push(temp.front());
          temp.pop();
     }

     // Send it back to the waiting queue.
     p.status = "READY";
     p.pushTime = pushTime++;
     waiting.push(p);
}

void handleWaiting()
{
     // Take process from waiting queue.
     process p = waiting.top();
     waiting.pop();

     if ( p.nextEvents.empty() )
     {
          // Terminate process.
          p.status = "TERMINATED";
          updateTable(p);

          // Output.
          cout << "\n";
          cout << "CURRENT STATES OF THE SYSTEM AT t = " << clockTime << " ms:" << endl;
          cout << "Current number of busy cores: " << nCores - coresAvailable << endl;
          cout << "READY QUEUE:" << endl;
          cout << "empty" << endl;
          cout << "DISK QUEUE:" << endl;
          cout << "empty" << endl;
          cout << "PROCESS TABLE:" << endl;
          for ( map<int, process>::iterator
              it = processTable.begin();
              it != processTable.end();
              it++ )
          {
               cout << "Process " << it->second.id << " started at " << it->second.startTime << " ms and is " << it->second.status << endl;
          }
          cout << endl;

          processTable.erase(p.id);

          return;
     }

     //cout << "handleWaiting " << p.nextEvents.front().operation << " at " << clockTime << endl;

     // Check if process cannot be started.
     if ( p.nextEvents.front().operation == "CORE" )
     {
          if ( coresAvailable == 0 )
          {
               // Prempt the process and exit this function.
               return preemptProcess(p);
          }
     }
     if ( p.nextEvents.front().operation == "DISK" )
     {
          if ( diskAvailable == false )
          {
               // Prempt the process and exit this function.
               return preemptProcess(p);
          }
     }

     // Start next event.
     if ( p.nextEvents.front().operation == "CORE" )
     {
          coresAvailable--;
     }
     if ( p.nextEvents.front().operation == "DISK" )
     {
          diskAvailable = false;
     }
     p.startEvent();
     updateTable(p);

     //cout << "Run until " << p.nextTime << endl;

     // Send it to the running queue.
     p.pushTime = pushTime++;
     running.push(p);
}

void handleRunning()
{
     // Take process from running queue.
     process p = running.top();
     running.pop();

     //cout << "handleRunning " << p.nextEvents.front().operation << " at " << clockTime << endl;

     // Move clockTime ahead.
     clockTime = p.nextTime;

     // Finish next event.
     if ( p.nextEvents.front().operation == "CORE" )
     {
          coresAvailable++;
     }
     if ( p.nextEvents.front().operation == "DISK" )
     {
          diskAvailable = true;
     }
     p.finishEvent();
     updateTable(p);

     p.pushTime = pushTime++;
     waiting.push(p);
}

void processInput()
{
     //read into the file, and create an input data table
     //ifstream file("input11.txt");
     ifstream file("1.txt");
     if ( !file.is_open() )
     {
          cout << "could not open file" << endl;
     }

     event e;
     process p;
     string line;
     while ( getline(file, line) )
     {
          stringstream str(line);
          str >> e.operation;
          str >> e.timeValue;

          if ( str.fail() )
          {
               continue;
          }

          if ( e.operation == "NCORES" )
          {
               nCores = e.timeValue;
               coresAvailable = nCores;
          }
          if ( e.operation == "NEW" )
          {
               if ( processesInMemory > 0 )
               {
                    // push the complete process onto the waiting queue
                    waiting.push(p);
                    processTable.insert(std::pair<int, process>(p.id, p));
                    // this will make p a new brand new process
                    p = process();
               }

               p.id = processesInMemory;
               p.startTime = e.timeValue;
               p.status = "CREATING";
               processesInMemory++;
               p.nextEvents.push(e);
          }
          if ( e.operation == "CORE" )
          {
               p.nextEvents.push(e);
          }
          if ( e.operation == "DISK" || e.operation == "DISPLAY" || e.operation == "INPUT" )
          {
               p.nextEvents.push(e);
          }
     }

     if ( processesInMemory > 0 )
     {
          // push the complete process onto the waiting queue
          p.pushTime = pushTime++;
          waiting.push(p);
          processTable.insert(std::pair<int, process>(p.id, p));
     }
}

int main()
{
     processInput();

     // While both queues are not empty.
     while ( !(waiting.empty() && running.empty()) )
     {
          // If one is empty, the other is not.
          if ( waiting.empty() )
          {
               handleRunning();
               continue;
          }
          // If one is empty, the other is not.
          if ( running.empty() )
          {
               handleWaiting();
               continue;
          }
          // If neither is empty
          // If nextTimes are equal
          if ( waiting.top().nextTime == running.top().nextTime )
          {
               // If nextEvent operations are equal
               if ( waiting.top().nextEvents.front().operation == running.top().nextEvents.front().operation )
               {
                    // Choose the running event.
                    handleRunning();
               }
               else
               {
                    // Choose the waiting event.
                    handleWaiting();
               }
               continue;
          }
          // If waiting nextTime comes first
          if ( waiting.top().nextTime < running.top().nextTime )
          {
               handleWaiting();
          }
          else
          {
               handleRunning();
          }
     }

     return 0;
}
