#include <iostream>                           // several standard stream objects
#include <string>                             // std::basic_string class template
#include <sstream>                            // std::basic_stringstream, std::basic_istringstream, std::basic_ostringstream class templates and several typedefs
#include <fstream>                            // std::basic_fstream, std::basic_ifstream, std::basic_ofstream class templates and several typedefs
#include <map>                                // std::map and std::multimap associative containers
#include <queue>                              // std::queue and std::priority_queue container adaptors
#include <iomanip>

using namespace std;

template<typename TO, typename FROM>
TO to(FROM const & a)
{
     TO b;
     stringstream ss;
     ss << a;
     ss >> b;
     return b;
}

namespace project
{
     string const TASK_STRING[]{ "NCORES", "NEW", "CORE", "DISK", "DISPLAY", "INPUT", "TERMINATE", "NONE" };
     enum struct TASK { NCORES, NEW, CORE, DISK, DISPLAY, INPUT, TERMINATE, NONE };
     string const STATE_STRING[]{ "RUNNING", "READY", "BLOCKED", "NONE" };
     enum struct STATE { RUNNING, READY, BLOCKED, NONE };

     /// DONE
     // { id, end_time }
     struct
          resource
     {
          int id;    int end_time;
     };

     /// DONE
     // { request, value }
     struct
          task
     {
          TASK request;    int value;
     };

     /// DONE
     // { id, tasklist, start_time, end_time, status }
     struct
          process
     {
          process(queue<task> & new_tasklist)
               : tasklist(new_tasklist)
          {
               id = 0;
               start_time = 0;
               end_time = 0;
               status = STATE::NONE;
          }

          int id;    queue<task> tasklist;    int start_time;    int end_time;    STATE status;
     };

     /// DONE
     class
          resource_table
     {
     public:
          resource_table() {}

          resource_table(int count)
               : resources(count, resource{ -1, -1 }) {}

          bool
               request
               (
                    process const & requesting_process
               )
          {
               // search for a row that has an '.end_time' less than the requesting
               // task's '.start_time' | if available, update the row and return
               // 'true' | if not available, return false
               for ( auto & r : resources )
               {
                    if ( r.end_time <= requesting_process.start_time )
                    {
                         r.id = requesting_process.id;
                         r.end_time = requesting_process.end_time;
                         return true;
                    }
               }
               return false;
          }

          int
               available_time()
          {
               int next_endtime = resources.at(0).end_time;

               auto && beg = resources.begin() + 1;
               auto && end = resources.end();
               for ( ; beg != end; ++beg )
               {
                    auto && r = *beg;

                    if ( r.end_time < next_endtime )
                    {
                         next_endtime = r.end_time;
                    }
               }

               return next_endtime;
          }

     private:
          vector<resource> resources;
     };

     class
          process_table
     {
     public:
          process_table() {}

          process_table(vector<process> && new_processes)
               : processes(new_processes) {}

          void
               print()
          {
               for ( auto & p : processes )
               {
                    cout << p.id << " " << p.start_time << " " << p.end_time << " " << STATE_STRING[static_cast<int>(p.status)] << endl;
                    queue<task> copy(p.tasklist);
                    while ( copy.empty() == false )
                    {
                         cout << "   " << TASK_STRING[static_cast<int>(copy.front().request)] << " " << copy.front().value << endl;
                         copy.pop();
                    }
               }
          }

     private:
          vector<process> processes;
     };

     // input file
     // {NEW,  t} creates new process at time t
     // {CORE, t} executes instructions for duration t
     // {DISK, 0} blocks disk access for 10ms
     // {DISK, 1} blocks disk access and process for 10ms
     // {DISPLAY, t} blocks process for duration t
     // {INPUT, t} blocks process for duration t

     /// TODO: do this later
     //tasklist
     //     build_from_stream
     //     (
     //          istream & input_stream
     //     )
     //{
     //     tasklist new_table;
     //
     //     string line;
     //     string TASK;
     //     int value;
     //
     //     while ( getline(input_stream, line) )
     //     {
     //          if ( line.empty() ) { continue; }
     //
     //          input_stream >> t;
     //          input_stream >> v;
     //
     //          task new_row;
     //          new_row.request = t;
     //          new_row.blocking = true;
     //          new_row.value = v;
     //
     //          if ( TASK == "NEW" )
     //          {
     //
     //          }
     //
     //          if ( TASK == "DISK" )
     //          {
     //               if ( value == 0 )
     //               {
     //                    new_row.blocking = false;
     //               }
     //               new_row.value = 10;
     //          }
     //     }
     //
     //     return new_table;
     //}

     /// excessively unnecessary class
     class
          builder_task
     {
     public:
          builder_task() = default;
          builder_task(const builder_task&) = delete;
          builder_task& operator=(const builder_task&) = delete;

          task
               row_via_jump_table
               (
                    tuple<string const, int const> const & pair
               )
          {
               s = get<0>(pair);         // TASK
               r.value = get<1>(pair);   // value
               char const & c = s.at(0); // first character of TASK

               // hash function: c % 4
               // used as index into jump tasklist
               (this->*jump[(c % 4)])();

               return r;
          }

     private:
          task r = { TASK::NONE, -1 };
          string & s = string();

          // { id    request    value    blocking    start_time    end_time }

          // DISK / DISPLAY
          void jump_D()
          {
               if ( s.at(3) == 'K' )
               {
                    r.request = TASK::DISK;
               }
               else
               {
                    r.request = TASK::DISPLAY;
               }
          };

          // INPUT
          void jump_I()
          {
               r.request = TASK::INPUT;
          };

          // NCORES / NEW
          void jump_N()
          {
               if ( s.at(1) == 'C' )
               {
                    r.request = TASK::NCORES;
               }
               else
               {
                    r.request = TASK::NEW;
               }
          };

          // CORE
          void jump_C()
          {
               r.request = TASK::CORE;
          };

          // Jump Table
          // D (68) % 4 = 0
          // I (73) % 4 = 1
          // N (78) % 4 = 2
          // C (67) % 4 = 3
          void(builder_task::* jump[4])() = { &builder_task::jump_D, &builder_task::jump_I, &builder_task::jump_N, &builder_task::jump_C };
     };

     /// DONE
     class
          builder_tasklist
     {
     public:
          static vector<process>
               build_via_jump_table
               (
                    istream & input_stream
               )
          {
               vector<process> process_table;
               builder_task bt;
               tuple<string, int> pair;

               process_table.push_back(process(queue<task>()));
               // discard anything that is not "NEW"
               while ( next_pair(pair, input_stream) )
               {
                    task t = bt.row_via_jump_table(pair);
                    if ( t.request == TASK::NEW )
                    {
                         process_table.back().tasklist.push(t);
                         break;
                    }
               }
               // process reamining lines
               while ( next_pair(pair, input_stream) )
               {
                    task t = bt.row_via_jump_table(pair);
                    if ( t.request == TASK::NEW )
                    {
                         process_table.push_back(process(queue<task>()));
                    }
                    process_table.back().tasklist.push(t);
               }

               return process_table;
          }
          static bool
               next_pair
               (
                    tuple<string, int> & pair,
                    istream & input_stream
               )
          {
               string line;
               while ( getline(input_stream, line) )
               {
                    if ( line.empty() ) { continue; }
                    stringstream ss(line);
                    ss >> get<0>(pair);
                    ss >> get<1>(pair);
                    return true;
               }
               return false;
          }
     };


     class
          machine
     {
     public:
          machine(ifstream && input_stream)
          {
               tuple<string, int> pair;
               builder_tasklist::next_pair(pair, input_stream);
               int ncores = get<1>(pair);

               ptable = process_table(builder_tasklist::build_via_jump_table(input_stream));
               cores = resource_table(ncores);
               disks = resource_table(1);

               input_stream.close();
          }

          bool
               step()
          {
               ptable.print();
               return true;
          }

     private:
          int system_time = 0;
          process_table ptable;
          resource_table cores;
          resource_table disks;
     };
}

int main(int argc, const char ** argv)
{
     using namespace project;

     machine m(ifstream("1.txt"));
     m.step();

     return 0;
}
