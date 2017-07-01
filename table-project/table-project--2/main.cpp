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

namespace EVENT_TABLE
{
     string TOKEN_STRING[] = { "NCORES", "NEW", "CORE", "DISK", "DISPLAY", "INPUT" };
     enum TOKEN { NCORES, NEW, CORE, DISK, DISPLAY, INPUT, TERMINATE, NONE };


     // { id, start_time, end_time }
     struct
          resource
     {
          int id;    int start_time;    int end_time;
     };

     // { id, request, value, blocking, start_time, end_time, order }
     struct
          task
     {
          int id;    TOKEN request;    int value;    bool blocking;    int start_time;    int end_time;    int order;

          bool operator()(task const & a, task const & b) const
          {
               //if ( a.start_time == b.start_time )
               //{
               //     return (a.order > b.order);
               //}
               return (a.start_time < b.start_time);
          }
     };


     class
          resource_table
     {
     public:
          resource_table(int count) : rows(count, resource{ -1, -1, -1 }) {};

          bool
               request
               (
                    task const & requesting_task,
                    int & next_free_time
               )
          {
               // search for a row that has an '.end_time' less than the requesting
               // task's '.start_time' | if available, update the row and return
               // 'true' | if not available, return false

               next_free_time = rows.at(0).end_time;

               for ( auto & r : rows )
               {
                    if ( r.end_time <= requesting_task.start_time )
                    {
                         r.id = requesting_task.id;
                         r.start_time = requesting_task.start_time;
                         r.end_time = requesting_task.end_time;
                         return true;
                    }
                    if ( r.end_time < next_free_time )
                    {
                         next_free_time = r.end_time;
                    }
               }

               return false;
          }

     private:
          vector<resource> rows;
     };

     class
          tasklist
     {
     public:
          tasklist(vector<task> && list_of_tasks)
          {
               // a comparator function similar to the one in struct task that takes
               // into consideration the 'id', as the 'order' is dependent upon it
               // up until this point
               struct temp_comparator
               {
                    bool operator()(task const & a, task const & b) const
                    {
                         if ( a.start_time == b.start_time )
                         {
                              if ( a.id == b.id )
                              {
                                   return (a.order > b.order);
                              }
                              return (a.id > b.id);
                         }
                         return (a.start_time > b.start_time);
                    }
               };

               // a temporary priority queue that utilizes the temperary comparator
               // function defined above
               priority_queue<task, vector<task>, temp_comparator> temp;

               // firstly, the tasks must taken from the vector and pushed into the
               // temporary priority queue defined above
               auto && beg = list_of_tasks.begin() + 1;
               auto && end = list_of_tasks.end();
               for ( ; beg != end; ++beg )
               {
                    auto && r = *beg;
                    temp.push(r);
               }

               // next, now that the tasks are properly sorted, they must be copied
               // from the temprory priority queue into the priority queue of this
               // tasklist object
               task p{ -1, NONE, -1, false, -1, -1, -1 };
               while ( temp.empty() == false )
               {
                    task t = temp.top();
                    temp.pop();

                    t.order = 0;
                    if ( t.start_time == p.start_time ) {
                         t.order = p.order + 1;
                    }
                    p = t;
                    tasks.push(t);
               }
          }

          void
               execute()
          {
               int number_of_cores = tasks.top().value;

               resource_table core(number_of_cores);
               resource_table disk(1);

               queue<task> completed_tasks;

               int count = 0;
               while ( tasks.empty() == false )
               {
                    count++;
                    task t = tasks.top();
                    tasks.pop();

                    switch ( t.request )
                    {
                         case NEW:
                         {
                              completed_tasks.push(t);
                         }
                         break;

                         case CORE:
                         {
                              int next_free_time = 0;
                              if ( core.request(t, next_free_time) == true )
                              {
                                   completed_tasks.push(t);
                              }
                              else
                              {
                                   cout << "core: " << next_free_time << endl;
                              }
                         }
                         break;

                         case DISK:
                         {
                              int next_free_time = 0;
                              if ( core.request(t, next_free_time) == true )
                              {
                                   completed_tasks.push(t);
                              }
                              else
                              {
                                   cout << "disk: " << next_free_time << endl;
                              }
                         }
                         break;

                         case DISPLAY:
                         {
                              completed_tasks.push(t);
                         }
                         break;

                         case INPUT:
                         {
                              completed_tasks.push(t);
                         }
                         break;

                         case TERMINATE:
                         {

                         }
                         break;
                    }
               }

               print(completed_tasks);
          }

          void
               print()
          {
               auto copy(tasks);
               while ( copy.empty() == false )
               {
                    task const & t = copy.top();
                    cout << setw(3) << t.id << " " << setw(10) << left << TOKEN_STRING[t.request] << " " << setw(5) << right << t.value << " " << setw(5) << t.start_time << " " << setw(5) << t.end_time << " " << t.blocking << " " << t.order << endl;
                    copy.pop();
               }
               cout << endl;
          }

          void
               print(queue<task> list)
          {
               auto copy(list);
               int count = 0;
               while ( copy.empty() == false )
               {
                    count++;
                    task const & t = copy.front();
                    cout << setw(3) << t.id << " " << setw(10) << left << TOKEN_STRING[t.request] << " " << setw(5) << right << t.value << " " << setw(5) << t.start_time << " " << setw(5) << t.end_time << " " << t.blocking << " " << t.order << endl;
                    copy.pop();
               }
               cout << endl;
               cout << count << endl;
          }



          bool
               validate()
          {
               // iterate the tasklist checking that the values of each task compared
               // to the previous task do not violate these rules:
               // 1) 

               // task { id, request, value, blocking, start_time, end_time, order }
               task p{ -1, NONE, -1, false, -1, -1, -1 };

               auto copy(tasks);
               while ( copy.empty() == false )
               {
                    task const & t = copy.top();
                    if ( t.start_time > t.end_time )
                    {
                         return false;
                    }
                    if ( t.start_time < p.start_time )
                    {
                         return false;
                    }
                    if ( t.order != 0 )
                    {
                         if ( t.id < p.id )
                         {
                              return false;
                         }
                    }
                    p = t;
                    copy.pop();
               }

               return true;
          }

     private:
          priority_queue<task, vector<task>, task> tasks;

          void
               time_adjust
               (
                    int delay
               )
          {
               // this function is called when a task cannot be executed and must
               // be delayed until the resource it requires is nextly available

               // to delay a task, 'delay' must be added to its '.start_time' and
               // '.end_time' | this adjustment will consequently break the ordering
               // of subsequent tasks | therefore, the '.start_time' and '.end_time'
               // for each subsequent task with the same '.id' must also be adjusted
               // with 'delay'

               // here is the process to do so:

               // 1) record the '.id' of the top task

               int id = tasks.top().id;

               // 2) create a temporary task object to store the values of previously
               //    processed tasks during the operation

               // task { id, request, value, blocking, start_time, end_time, order }
               task p{ 0, NONE, 0, false, 0, 0, 0 };

               // 3) step through the tasklist adding 'delay' to the '.start_time'
               //    and '.end_time' of each task with '.id' equal to 'id' | reset
               //    the '.order' for each task as well, regardless of 'id' | then
               //    increment the '.order' whenever the task's '.start_time' is
               //    equal to the previous task's '.start_time'

               // 4) push each task into a temporary priority_queue for obvious
               //    reasons | after the operation is finished, swap the data of the
               //    temporary priority queue with the data of the tasklist object

               priority_queue<task, vector<task>, task> temp;

               while ( tasks.empty() == false )
               {
                    auto t = tasks.top(); tasks.pop();

                    if ( t.id == id )
                    {
                         t.start_time += delay;
                         t.end_time += delay;
                    }

                    t.order = 0;
                    if ( t.start_time == p.start_time )
                    {
                         t.order = p.order + 1;
                    }

                    temp.push(t);
                    p = t;
               }

               tasks.swap(temp);
          }
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
     //     string token;
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
     //          if ( token == "NEW" )
     //          {
     //
     //          }
     //
     //          if ( token == "DISK" )
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
               s = get<0>(pair);         // token
               r.value = get<1>(pair);   // value
               char const & c = s.at(0); // first character of token

               // hash function: c % 4
               // used as index into jump tasklist
               (this->*jump[(c % 4)])();

               return r;
          }

     private:
          task r = { -1, NONE, true, 0, 0, 0, 0 };
          string & s = string();

          // { id    request    value    blocking    start_time    end_time }

          // DISK / DISPLAY
          void jump_D()
          {
               if ( s.at(3) == 'K' )
               {
                    r.request = DISK;
                    if ( r.value == 0 ) { r.blocking = false; }
                    r.value = 10;
               }
               else
               {
                    r.request = DISPLAY;
                    r.blocking = true;
               }
          };

          // INPUT
          void jump_I()
          {
               r.request = INPUT;
               r.blocking = true;
          };

          // NCORES / NEW
          void jump_N()
          {
               if ( s.at(1) == 'C' )
               {
                    r.request = NCORES;
                    r.blocking = true;
               }
               else
               {
                    r.id = r.id + 1;
                    r.request = NEW;
                    r.blocking = true;
               }
          };

          // CORE
          void jump_C()
          {
               r.request = CORE;
               r.blocking = true;
          };

          // Jump Table
          // D (68) % 4 = 0
          // I (73) % 4 = 1
          // N (78) % 4 = 2
          // C (67) % 4 = 3
          void(builder_task::* jump[4])() = { &builder_task::jump_D, &builder_task::jump_I, &builder_task::jump_N, &builder_task::jump_C };
     };

     class
          builder_tasklist
     {
     public:
          static vector<task>
               build_via_jump_table
               (
                    istream & input_stream
               )
          {
               vector<task> rows;

               builder_task br;
               tuple<string, int> pair;
               while ( next_pair(pair, input_stream) )
               {
                    rows.push_back(br.row_via_jump_table(pair));
               }

               initial_time_adjust(rows);
               return rows;
          }
     private:
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

          static void
               initial_time_adjust
               (
                    vector<task> & rows
               )
          {
               // { id    request    value    blocking    start_time    end_time    order }
               task p = { 0, NONE, 0, false, 0, 0, 0 };

               auto && beg = rows.begin();
               auto && end = rows.end();
               for ( ; beg != end; ++beg )
               {
                    auto && r = *beg;

                    if ( r.request == NEW )
                    {
                         r.start_time = r.value;
                         r.end_time = r.value;
                    }
                    else
                    {
                         r.start_time = p.end_time;
                         r.end_time = r.start_time + r.value;
                         if ( r.start_time == p.start_time )
                         {
                              r.order = p.order + 1;
                         }
                    }
                    p = r;
               }
          }
     };
};

int main(int argc, const char ** argv)
{
     using namespace EVENT_TABLE;

     tasklist t = builder_tasklist::build_via_jump_table(ifstream("1.txt"));
     //t.execute();
     t.print();
     cout << ((t.validate() == true) ? "true" : "false") << endl;
     //t.time_adjust(6);
     //t.print();

     return 0;
}
