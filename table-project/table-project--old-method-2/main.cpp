//Ibrahim Kobeissi
//COSC 3360
//PROFESSOR JEHAN F. PARIS
//SPRING 2017


#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <utility>

using namespace std;

enum processStatus { ready = 0, running = 1, blocked = 2, terminated = 3, notStarted = 4 };


struct processes
{
     int startTime;
     int coreTime;
     int processNumber;
     queue<string> processType;
     queue<int> processTime;
     processStatus currentStatus;
};

int globalTime = 0;
int cores = 0;
//int timeSlice = 0;
int processIndex = -1;
int processDone = -1;


int main(int argc, char* argv[])
{
     string eventType;
     string event;
     int time;
     deque<processes> readyQueue;
     deque<processes> diskQueue;
     deque<processes> disk;
     vector<processes> theProcesses(64);

     istream & cin = ifstream("2.txt");

     if ( cin.fail() )
     {
          cout << "Invalid input file! Please try again with a valid input file." << endl;
     }

     cin >> event;

     if ( event == "NCORES" )
     {
          cin >> cores;
          //cout << "The number of cores: " << cores << endl;
     }

     //cin >> event;

     //if ( event == "SLICE" )
     //{
     //     cin >> timeSlice;
     //     //cout << "The time slice: " << timeSlice << endl; // Remove this later, this is just to make sure the time slice is correctly input;
     //}

     while ( cin )
     {
          cin >> event;
          cin >> time;
          eventType = event;
          if ( event == "NEW" )
          {
               processIndex++;
               theProcesses.at(processIndex).startTime = time;
               theProcesses.at(processIndex).currentStatus = notStarted;
               theProcesses.at(processIndex).processNumber = processIndex;
          }

          if ( event == "CORE" )
          {
               theProcesses.at(processIndex).processType.push(eventType);
               theProcesses.at(processIndex).processTime.push(time);
          }

          if ( event == "DISK" )
          {
               theProcesses.at(processIndex).processType.push(eventType);
               theProcesses.at(processIndex).processTime.push(time);

          }

          if ( event == "DISPLAY" )
          {
               theProcesses.at(processIndex).processType.push(eventType);
               theProcesses.at(processIndex).processTime.push(time);
          }

          if ( event == "INPUT" )
          {
               theProcesses.at(processIndex).processType.push(eventType);
               theProcesses.at(processIndex).processTime.push(time);
          }
     }
     cout << endl;
     for ( int processNum = 0; processNum < processIndex + 1; processNum++ )
     {
          cout << "Process" << theProcesses.at(processNum).processNumber << " " << "Starts at: " << theProcesses.at(processNum).startTime << endl;
          cout << "With the first process being: " << theProcesses.at(processNum).processType.front() << " " << "For: " << theProcesses.at(processNum).processTime.front() << " " << "MS" << endl;
     }

     for ( int processNum = 0; processNum < processIndex + 1; processNum++ )
     {
          readyQueue.push_back(theProcesses.at(processNum));
          readyQueue.at(processNum).currentStatus = ready;
     }



     deque<processes> blockedQueue;
     deque<processes> terminatedQueue;
     vector<processes> processor; //actual core holder // use processor.size() to get the size, and based off of that you can tell if the cores are busy.
     int t = 0;


     while ( processDone != processIndex )
          //while (!readyQueue.empty())
     {
          if ( !terminatedQueue.empty() )
          {
               cout << endl;
               {
                    cout << "THE CURRENT STATE OF THE SYSTEM AT t = " << globalTime << endl;
                    if ( processor.empty() )
                    {
                         cout << "Number of busy cores: " << "0" << endl;
                    }
                    else
                    {
                         cout << "Number of busy cores: " << processor.size() << endl;;
                    }

                    if ( readyQueue.empty() )
                    {
                         cout << "READY QUEUE: " << endl;
                         cout << "empty" << endl;
                    }
                    else
                    {
                         cout << "READY QUEUE: " << endl;
                         for ( int pNum = 0; pNum < readyQueue.size(); pNum++ )
                         {
                              cout << "process" << readyQueue.at(pNum).processNumber << endl;
                         }
                    }
                    if ( diskQueue.empty() )
                    {
                         cout << "DISK QUEUE: " << endl;
                         cout << "empty" << endl;
                    }
                    else
                    {
                         cout << "DISK QUEUE: " << endl;
                         for ( int pNum = 0; pNum < diskQueue.size(); pNum++ )
                         {
                              cout << "process" << diskQueue.at(pNum).processNumber << endl;
                         }
                    }
                    cout << "PROCESS TABLE: " << endl;
                    {
                         if ( !terminatedQueue.empty() )
                         {
                              for ( int j = 0; j < terminatedQueue.size(); j++ )
                              {
                                   cout << "Process " << terminatedQueue.at(j).processNumber << " " << "Started at " << terminatedQueue.at(j).startTime << " " << "MS" << " " << "and is: ";
                                   if ( terminatedQueue.at(j).currentStatus == 0 )
                                   {
                                        cout << " " << "Ready." << endl;
                                   }
                                   else if ( terminatedQueue.at(j).currentStatus == 1 )
                                   {
                                        cout << " " << "Running." << endl;
                                   }
                                   else if ( terminatedQueue.at(j).currentStatus == 2 )
                                   {
                                        cout << " " << "Blocked." << endl;
                                   }
                                   else if ( terminatedQueue.at(j).currentStatus == 3 )
                                   {
                                        cout << " " << "Terminated." << endl;
                                   }
                              }
                         }
                         if ( !diskQueue.empty() )
                         {
                              for ( int j = 0; j < diskQueue.size(); j++ )
                              {
                                   cout << "Process " << diskQueue.at(j).processNumber << " " << "Started at " << diskQueue.at(j).startTime << " " << "MS" << " " << "and is:";
                                   if ( diskQueue.at(j).currentStatus == 0 )
                                   {
                                        cout << " " << "Ready." << endl;
                                   }
                                   else if ( diskQueue.at(j).currentStatus == 1 )
                                   {
                                        cout << " " << "Running." << endl;
                                   }
                                   else if ( diskQueue.at(j).currentStatus == 2 )
                                   {
                                        cout << " " << "Blocked." << endl;
                                   }
                                   else if ( diskQueue.at(j).currentStatus == 3 )
                                   {
                                        cout << " " << "Terminated." << endl;
                                   }
                              }
                         }
                         if ( !readyQueue.empty() )
                         {
                              for ( int j = 0; j < readyQueue.size(); j++ )
                              {
                                   cout << "Process " << readyQueue.at(j).processNumber << " " << "Started at " << readyQueue.at(j).startTime << " " << "MS" << " " << "and is:";
                                   if ( readyQueue.at(j).currentStatus == 0 )
                                   {
                                        cout << " " << "Ready." << endl;
                                   }
                                   else if ( readyQueue.at(j).currentStatus == 1 )
                                   {
                                        cout << " " << "Running." << endl;
                                   }
                                   else if ( readyQueue.at(j).currentStatus == 2 )
                                   {
                                        cout << " " << "Blocked." << endl;
                                   }
                                   else if ( readyQueue.at(j).currentStatus == 3 )
                                   {
                                        cout << " " << "Terminated." << endl;
                                   }
                              }
                         }
                         if ( !blockedQueue.empty() )
                         {
                              for ( int j = 0; j < blockedQueue.size(); j++ )
                              {
                                   cout << "Process " << blockedQueue.at(j).processNumber << " " << "Started at " << blockedQueue.at(j).startTime << " " << "MS" << " " << "and is:";
                                   if ( blockedQueue.at(j).currentStatus == 0 )
                                   {
                                        cout << " " << "Ready." << endl;
                                   }
                                   else if ( blockedQueue.at(j).currentStatus == 1 )
                                   {
                                        cout << " " << "Running." << endl;
                                   }
                                   else if ( blockedQueue.at(j).currentStatus == 2 )
                                   {
                                        cout << " " << "Blocked." << endl;
                                   }
                                   else if ( blockedQueue.at(j).currentStatus == 3 )
                                   {
                                        cout << " " << "Terminated." << endl;
                                   }
                              }
                         }
                         if ( !disk.empty() )
                         {
                              cout << "Process " << disk.front().processNumber << " " << "Started at " << disk.front().startTime << " " << "MS" << " " << "and is:";
                              if ( disk.front().currentStatus == 0 )
                              {
                                   cout << " " << "Ready." << endl;
                              }
                              else if ( disk.front().currentStatus == 1 )
                              {
                                   cout << " " << "Running." << endl;
                              }
                              else if ( disk.front().currentStatus == 2 )
                              {
                                   cout << " " << "Blocked." << endl;
                              }
                              else if ( disk.front().currentStatus == 3 )
                              {
                                   cout << " " << "Terminated." << endl;
                              }
                         }
                         if ( !processor.empty() )
                         {
                              for ( int j = 0; j < processor.size(); j++ )
                              {
                                   cout << "Process " << processor.at(j).processNumber << " " << "Started at " << processor.at(j).startTime << " " << "MS" << " " << "and is:";
                                   if ( processor.at(j).currentStatus == 0 )
                                   {
                                        cout << " " << "Ready." << endl;
                                   }
                                   else if ( processor.at(j).currentStatus == 1 )
                                   {
                                        cout << " " << "Running." << endl;
                                   }
                                   else if ( processor.at(j).currentStatus == 2 )
                                   {
                                        cout << " " << "Blocked." << endl;
                                   }
                                   else if ( processor.at(j).currentStatus == 3 )
                                   {
                                        cout << " " << "Terminated." << endl;
                                   }
                              }
                         }
                    }
               }
               terminatedQueue.pop_front();
               processDone++;
          }
          else
          {
          label1:
               if ( !readyQueue.empty() )
               {
                    if ( readyQueue.front().processType.front() == "CORE" && processor.size() != cores )
                    {
                         globalTime += readyQueue.front().startTime;
                         readyQueue.front().currentStatus = running;
                         processor.push_back(readyQueue.front());
                         readyQueue.pop_front();
                         for ( int i = 0; i < processor.size(); i++ )
                         {
                              //if ( processor.at(i).processTime.front() > timeSlice )
                              //{
                              //     globalTime += processor.at(i).processTime.front();
                              //     processor.at(i).processTime.front() = processor.at(i).processTime.front() - timeSlice;
                              //     processor.at(i).currentStatus = ready;
                              //     readyQueue.push_back(processor.at(i));
                              //     processor.erase(processor.begin() + i);
                              //}
                              //else
                              {
                                   globalTime += processor.at(i).processTime.front();
                                   processor.at(i).processType.pop();
                                   processor.at(i).processTime.pop();
                                   if ( processor.at(i).processType.empty() )
                                   {
                                        processor.at(i).currentStatus = terminated;
                                        terminatedQueue.push_back(processor.at(i));
                                        processor.erase(processor.begin() + i);
                                        goto label1;
                                   }
                                   if ( processor.at(i).processType.front() == "DISK" )
                                   {
                                        processor.at(i).currentStatus = blocked;
                                        diskQueue.push_back(processor.at(i));
                                        processor.erase(processor.begin() + i);
                                   }
                                   else if ( processor.at(i).processType.front() == "INPUT" || processor.at(i).processType.front() == "DISPLAY" )
                                   {
                                        processor.at(i).currentStatus = blocked;
                                        blockedQueue.push_back(processor.at(i));
                                        processor.erase(processor.begin() + i);
                                   }
                                   else
                                   {
                                        processor.at(i).currentStatus = ready;
                                        readyQueue.push_back(processor.at(i));
                                        processor.erase(processor.begin() + i);
                                   }
                              }
                         }
                    }
                    if ( !diskQueue.empty() )
                    {
                         if ( diskQueue.front().processType.front() == "DISK" )
                         {
                              while ( disk.empty() && !diskQueue.empty() )
                              {
                                   disk.push_front(diskQueue.front());
                                   diskQueue.pop_front();
                                   globalTime += disk.front().processTime.front();
                                   disk.front().processType.pop();
                                   disk.front().processTime.pop();
                                   if ( disk.front().processType.empty() )
                                   {
                                        disk.front().currentStatus = terminated;
                                        terminatedQueue.push_back(disk.front());
                                        disk.pop_front();
                                        goto label1;
                                   }
                                   if ( disk.front().processType.front() == "INPUT" || disk.front().processType.front() == "DISPLAY" )
                                   {
                                        blockedQueue.push_back(disk.front());
                                        disk.pop_front();
                                   }
                                   else
                                   {
                                        disk.front().currentStatus = ready;
                                        readyQueue.push_back(disk.front());
                                        disk.pop_front();
                                   }

                              }
                         }

                    }
                    if ( !blockedQueue.empty() )
                    {
                         if ( blockedQueue.front().processType.front() == "DISPLAY" || blockedQueue.front().processType.front() == "INPUT" )
                         {
                              globalTime += blockedQueue.front().processTime.front();
                              blockedQueue.front().processType.pop();
                              blockedQueue.front().processTime.pop();
                              if ( blockedQueue.front().processType.empty() )
                              {
                                   blockedQueue.front().currentStatus = terminated;
                                   terminatedQueue.push_back(blockedQueue.front());
                                   blockedQueue.pop_front();
                                   goto label1;
                              }
                              blockedQueue.front().currentStatus = ready;
                              readyQueue.push_back(blockedQueue.front());
                              blockedQueue.pop_front();
                         }
                    }
               }
          }
     }
     return 0;
}