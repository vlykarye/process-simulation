// Original Author
//   Larry Carranco
//   COSC 3360 - Operating Systems

// Rewritten by
//   Vlykarye


#include "stdafx.h"

using namespace std;

namespace sim
{
     // TASK enumeration

     enum class
          TASK
     {
          NCORES, NEW, CORE, DISK, DISPLAY, INPUT, TERMINATE
     };

     string const
          TASK_STRING[] =
     {
          "NCORES", "NEW", "CORE", "DISK", "DISPLAY", "INPUT", "TERMINATE"
     };

     TASK to_task(string str)
     {
          // ordered by frequency
          if ( str == "CORE" ) { return TASK::CORE; }
          if ( str == "DISK" ) { return TASK::DISK; }
          if ( str == "DISPLAY" ) { return TASK::DISPLAY; }
          if ( str == "INPUT" ) { return TASK::INPUT; }
          if ( str == "NEW" ) { return TASK::NEW; }
          if ( str == "NCORES" ) { return TASK::NCORES; }

          throw exception("task does not exist");
     }

     string to_string(TASK task)
     {
          return TASK_STRING[static_cast<int>(task)];
     }

     // STATE enumeration

     enum class
          STATE
     {
          RUNNING, READY, BLOCKED, TERMINATED
     };

     string const
          STATE_STRING[] =
     {
          "RUNNING", "READY", "BLOCKED", "TERMINATED"
     };

     STATE to_state(string str)
     {
          // ordered by frequency
          if ( str == "BLOCKED" ) { return STATE::BLOCKED; }
          if ( str == "READY" ) { return STATE::READY; }
          if ( str == "RUNNING" ) { return STATE::RUNNING; }
          if ( str == "TERMINATED" ) { return STATE::TERMINATED; }

          throw exception("task does not exist");
     }

     string to_string(STATE state)
     {
          return STATE_STRING[static_cast<int>(state)];
     }

     // High Level Objects

     class task
     {
     public:
          task(string task_token, int task_value)
          {
               token = to_task(task_token);
               value = task_value;
          }

          task(tuple<string, int> pair)
          {
               token = to_task(get<0>(pair));
               value = get<1>(pair);
          }

          TASK token;
          unsigned int value;
     };

     class process
     {
     public:
          process(int id)
          {
               pid = id;
          }

          int id;
          int start_time;
          int end_time;
          queue<task> tasks;
          STATE state;
     };

     class Builder_TaskList
     {
     public:
          Builder_TaskList() = default;
          Builder_TaskList(const Builder_TaskList &) = default;
          Builder_TaskList &operator=(const Builder_TaskList &) = default;
          ~Builder_TaskList() = default;

          static bool next_pair(tuple<string, int> & pair, istream & input_stream)
          {
               string line;
               while ( getline(input_stream, line) )
               {
                    if ( line.empty() )
                    {
                         continue;
                    }
                    stringstream ss(line);
                    ss >> get<0>(pair);
                    ss >> get<1>(pair);
                    return true;
               }
               return false;
          }

          static void build_process_table(istream & input_stream, vector<Process> & process_table)
          {
               tuple<string, int> pair;

               queue<Task> tl;
               int pid = 0;
               // discard anything that is not "NEW"
               while ( next_pair(pair, input_stream) )
               {
                    Task t(pair);
                    if ( t.task_name == TASKS::NEW )
                    {
                         tl.push(t);
                         break;
                    }
               }
               // Process reamining lines
               while ( next_pair(pair, input_stream) )
               {
                    Task t(pair);
                    if ( t.task_name == TASKS::NEW )
                    {
                         Process p(pid);
                         pid++;
                         tuple<string, int> newp;
                         get<0>(newp) = "TERMINATE";
                         get<1>(newp) = 0;
                         tl.push(Task(newp));
                         p.tasks = tl; // Copying queue into process
                         process_table.push_back(p);
                         tl = queue<Task>(); // Clearing queue
                    }
                    tl.push(t);
               }
               Process p(pid);
               pid++;
               tuple<string, int> newp;
               get<0>(newp) = "TERMINATE";
               get<1>(newp) = 0;
               tl.push(Task(newp));
               p.tasks = tl; // Copying queue into process
               process_table.push_back(p);
               tl = queue<Task>(); // Clearing queue
          }

     private:

     };

     class ProcessTable
     {
     public:
          ProcessTable() = default;
          ProcessTable(const ProcessTable &) = default;
          ProcessTable &operator=(const ProcessTable &) = default;
          ~ProcessTable() = default;

          void init()
          {
               for ( auto & p : processes )
               {
                    p.start_time = p.tasks.front().request_time;
                    p.end_time = p.tasks.front().request_time;
               }
          }

          bool get(int time, TASKS task, int & pid)
          {
               for ( auto & p : processes )
               {
                    if ( p.tasks.front().task_name == task )
                    {
                         if ( p.start_time == time )
                         {
                              pid = p.pid;
                              return true;
                         }
                    }
               }
               return false;
          }

          void update(int pid, STATES new_state)
          {
               processes.at(pid).state = new_state;
          }

          bool pop_task(int pid)
          {
               Process & p = processes.at(pid);
               if ( p.tasks.empty() == false )
               {
                    p.end_time += p.tasks.front().request_time;
                    p.tasks.pop();
                    return true;
               }
               return false;
          }

          Task get_task(int pid)
          {
               return processes.at(pid).tasks.front();
          }

          STATES get_state(int pid)
          {
               return processes.at(pid).state;
          }

          bool min_start(int & pid)
          {
               bool result = false;
               int min = INT_MAX;

               for ( auto & p : processes )
               {
                    if ( get_task(p.pid).task_name == TASKS::NEW )
                    {
                         if ( p.start_time < min )
                         {
                              min = p.start_time;
                              pid = p.pid;
                              result = true;
                         }
                    }
               }

               return result;
          }

          int min_end()
          {
               int min = processes.at(0).end_time;
               int pid = processes.at(0).pid;

               auto & beg = processes.begin();
               auto & end = processes.end();
               for ( ; beg != end; ++beg )
               {
                    auto & p = *beg;
                    if ( p.tasks.front().task_name == TASKS::CORE && p.state == STATES::READY )
                    {
                         if ( p.end_time < min )
                         {
                              min = p.end_time;
                              pid = p.pid;
                         }
                    }
               }

               return pid;
          }

          vector<Process> processes;

     private:

     };

     class Resource
     {
     public:
          Resource() = default;
          Resource(const Resource &) = default;
          Resource &operator=(const Resource &) = default;
          ~Resource() = default;

          int pid = 0;
          int end_time = 0;
          bool busy = false;

     private:

     };

     class ResourceTable
     {
     public:
          ResourceTable() = default;
          ResourceTable(const ResourceTable &) = default;
          ResourceTable &operator=(const ResourceTable &) = default;
          ~ResourceTable() = default;

          bool check(int time, int pid, int end_time)
          {
               for ( auto & r : resources )
               {
                    if ( r.end_time <= time )
                    {
                         return true;
                    }
               }
               return false;
          }

          void load(int time, int pid, int end_time)
          {
               for ( auto & r : resources )
               {
                    if ( r.end_time <= time )
                    {
                         r.pid = pid;
                         r.end_time = end_time;
                         r.busy = true;
                         ++count;
                         return;
                    }
               }
          }

          bool get_finished(int time, int & pid)
          {
               for ( auto & r : resources )
               {
                    if ( r.end_time == time )
                    {
                         pid = r.pid;
                         r.pid = -1;
                         r.end_time = 0;
                         r.busy = false;
                         return true;
                    }
               }
               return false;
          }

          vector<Resource> resources;
          int count = 0;

     private:

     };

     class Machine
     {
     public:
          Machine() = default;
          Machine(ifstream && input_stream)
          {
               tuple<string, int> pair;
               Builder_TaskList::next_pair(pair, input_stream);
               int ncores = get<1>(pair);
               Builder_TaskList::build_process_table(input_stream, pt.processes);
               core.resources = vector<Resource>(ncores);
               disk.resources = vector<Resource>(1);
               pt.init();
          }

          Machine &operator=(const Machine &) = default;
          ~Machine() = default;

          void executing_tasks()
          {
               while ( pt.processes.empty() == false )
               {
                    int pid = pt.min_end();
                    TASKS task = pt.get_task(pid).task_name;
                    STATES state = pt.get_state(pid);

                    // load processes that haven't loaded yet
                    if ( task == TASKS::NEW )
                    {
                         current_time = pt.processes.at(pid).start_time;
                         loading_process();
                         continue;
                    }

                    current_time = pt.processes.at(pid).end_time;


                    // move to core
                    // check every time
                    if ( task == TASKS::CORE && state == STATES::READY )
                    {
                         process_from_ready_queue_to_core();
                    }

                    if ( state == STATES::RUNNING )
                    {
                         // move out of core
                         process_from_core_to_disk_queue();
                    }

                    // move to disk
                    // check every time
                    if ( task == TASKS::DISK && state == STATES::BLOCKED )
                    {
                         process_from_disk_queue_to_disk();
                    }

                    if ( state == STATES::BLOCKED )
                    {
                         // move out of disk
                         process_from_disk_to_ready_queue();
                    }



                    //process_from_blocked_state_to_ready_queue();

                    process_from_core_to_terminated();
               }
          }

          // 1
          void loading_process()
          {
               int pid;
               if ( pt.get(current_time, TASKS::NEW, pid) )
               {
                    pt.update(pid, STATES::READY);
                    pt.pop_task(pid);
                    readyq.push(pid);
                    cout << "starting process " << pid << endl;
               }
          }

          // 2
          void process_from_ready_queue_to_core()
          {
               while ( readyq.empty() == false )
               {
                    int pid = readyq.front();

                    if ( core.check(current_time, pid, pt.processes.at(pid).end_time) )
                    {
                         readyq.pop();
                         pt.update(pid, STATES::RUNNING);
                         pt.pop_task(pid);
                         core.load(current_time, pid, pt.processes.at(pid).end_time);
                         cout << "loading process " << pid << " into core" << endl;
                    }
                    else return;
               }
          }

          // 3 4 5
          void process_from_core_to_disk_queue()
          {
               int pid;
               while ( core.get_finished(current_time, pid) )
               {
                    TASKS task = pt.get_task(pid).task_name;
                    switch ( task )
                    {
                         case TASKS::CORE:
                              pt.update(pid, STATES::READY);
                              readyq.push(pid);
                              cout << "moving process " << pid << " into readyq" << endl;
                              break;
                         case TASKS::DISK:
                              pt.update(pid, STATES::BLOCKED);
                              diskq.push(pid);
                              cout << "moving process " << pid << " into diskq" << endl;
                              break;
                         case TASKS::DISPLAY:
                         case TASKS::INPUT:
                              pt.update(pid, STATES::BLOCKED);
                              cout << "input/display process " << pid << endl;
                              // TODO
                              break;
                    }
               }
          }

          // 6
          void process_from_disk_queue_to_disk()
          {
               while ( diskq.empty() == false )
               {
                    int pid = diskq.front();
                    if ( disk.check(current_time, pid, pt.processes.at(pid).end_time) )
                    {
                         diskq.pop();
                         pt.update(pid, STATES::BLOCKED);
                         pt.pop_task(pid);
                         disk.load(current_time, pid, 10);
                         cout << "loading process " << pid << " into disk" << endl;
                    }
                    else return;
               }
          }

          // 7
          void process_from_disk_to_ready_queue()
          {
               int pid;
               while ( disk.get_finished(current_time, pid) )
               {
                    TASKS task = pt.get_task(pid).task_name;
                    switch ( task )
                    {
                         case TASKS::CORE:
                              pt.update(pid, STATES::READY);
                              readyq.push(pid);
                              cout << "moving process " << pid << " into readyq from disk" << endl;
                              break;
                         case TASKS::DISK:
                              pt.update(pid, STATES::BLOCKED);
                              diskq.push(pid);
                              cout << "moving process " << pid << " into diskq from disk" << endl;
                              break;
                         case TASKS::DISPLAY:
                         case TASKS::INPUT:
                              pt.update(pid, STATES::BLOCKED);
                              cout << "input/display process " << pid << " from disk" << endl;
                              // TODO
                              break;
                    }
               }
          }

          // 8
          void process_from_blocked_state_to_ready_queue()
          {
               // Cover somewhere else
          }

          // 9
          void process_from_core_to_terminated()
          {
               int pid;
               while ( core.get_finished(current_time, pid) )
               {
                    pt.update(pid, STATES::TERMINATED);
                    pt.pop_task(pid);
                    cout << "terminate process " << pid << endl;
               }
          }

          // 10
          void print_report()
          {
               // TODO
          }

          ProcessTable pt;
          queue<int> readyq;
          queue<int> diskq;
          ResourceTable core;
          ResourceTable disk;
          int current_time = 0;

     private:

     };

} // End namespace

int main(int argc, char *argv[])
{
     // if(argc < 2) {
     //    //cerr("Usage: process \"filename=input.txt;\"\n");
     //    return 1;
     // }
     // ArgumentManager am(argc, argv);
     // string filename = am.get("filename");

     // Used for debugging on Visual Studio
     Hw1::Machine m(ifstream("1.txt"));
     m.executing_tasks();


     // You MUST use I/O redirection
          // assign1 < input_file
     // Advantages
          // Very flexible
          // Programmer writes her code as if in was read from standard input
               // Much easier than dealing with files

     return 0;
}