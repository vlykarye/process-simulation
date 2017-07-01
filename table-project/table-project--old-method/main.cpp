/*
    Eric Nalley (1395862)
    COSC 3360
*/

// g++ -std=c++11 -o main -I ./ *.cpp

#include <string>
#include <list>
#include <set>
#include <map>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;
typedef unsigned int uint;

namespace ext
{
     // This class extends the functionality of the standard set class.
     // Only add new member functions. Do not add new data members.
     template <class T>
     class set
          : public std::set<T>
     {
     public:
          // Boiler Plate Code
          // use base class constructors
          using std::set<T>::set;
          // convert std::set to ext::set if needed
          set<T>(std::set<T> S) : std::set<T>(S) {}
          // New Member Functions
          bool contains(T const & input) const
          {
               // success: find() != end()
               // failure: find() == end()
               return std::set<T>::find(input) != std::set<T>::end();
          }
     };
}

//	entities
//		operation
//			process id
//			type
//			time value
struct class_event
{
     enum enum_type
     {
          NEW,
          CORE,
          DISK,
          DISPLAY,
          INPUT,
          QUIT
     };
     enum_type type() { return m_type; }
     string type_string()
     {
          switch ( m_type )
          {
               case NEW:
                    return "NEW";
               case CORE:
                    return "CORE";
               case DISK:
                    return "DISK";
               case DISPLAY:
                    return "DISPLAY";
               case INPUT:
                    return "INPUT";
               case QUIT:
                    return "QUIT";
          }
          return "ERROR";
     }

     int debug()
     {
          switch ( m_type )
          {
               case NEW:
                    return 0;
               case CORE:
                    return 0;
               case DISK:
                    return 0;
               case DISPLAY:
                    return 0;
               case INPUT:
                    return 0;
               case QUIT:
                    return 0;
          }
          return 0;
     }

     struct
     {
          friend class_event;

          uint request() { return m_request; }
          uint started() { return m_started; }
          uint stop_at() { return m_request + m_started; }

     private:
          void request(uint t) { m_request = t; }
          void started(uint t) { m_started = t; }

          uint m_request = 0;
          uint m_started = 0;
     } time;

     class_event(enum_type type, uint time)
          : m_type(type)
     {
          this->time.request(time);
     }

     void start(uint t)
     {
          time.started(t);

          if ( debug() )
          {
               cout << "event " << type_string() << " at " << t << " ms" << endl;
          }
     }

private:
     enum_type m_type;
};

//		process
//			id
//			start time
//			elapsed time
//			state
//				READY
//				RUNNING
//				BLOCKED
//				TERMINATED
//			operation queue
struct class_process
{
     enum enum_state
     {
          ZERO,
          READY,
          RUNNING,
          BLOCKED,
          TERMINATED
     };
     enum_state state() { return m_state; }
     string state_string()
     {
          switch ( m_state )
          {
               case       ZERO:
                    return "ZERO";
               case      READY:
                    return "READY";
               case    RUNNING:
                    return "RUNNING";
               case    BLOCKED:
                    return "BLOCKED";
               case TERMINATED:
                    return "TERMINATED";
          }
          return "ERROR";
     }

     int debug()
     {
          switch ( events.next().type() )
          {
               case class_event::NEW:
                    return 0;
               case class_event::CORE:
                    return 0;
               case class_event::DISK:
                    return 0;
               case class_event::DISPLAY:
                    return 0;
               case class_event::INPUT:
                    return 0;
               case class_event::QUIT:
                    return 0;
          }
          return 0;
     }


     struct
     {
          uint elapsed() { return m_elapsed; }
          uint started() { return m_started; }
          uint stopped() { return m_stopped; }
          uint core_use() { return m_core; }
          uint disk_use() { return m_disk; }

          void elapsed(uint t) { m_elapsed = t; }
          void started(uint t) { m_started = t; }
          void stopped(uint t) { m_stopped = t; }
          void core(uint t) { m_core = t; }
          void disk(uint t) { m_disk = t; }

     private:
          uint m_elapsed = 0;
          uint m_started = 0;
          uint m_stopped = 0;
          uint m_core = 0;
          uint m_disk = 0;
     } time;

     struct
     {
          friend class_process;

          class_event & next() { return upcoming.front(); }

          uint pending() { return upcoming.size(); }
          uint completed() { return complete.size(); }

     private:
          queue<class_event> upcoming;
          queue<class_event> complete;
     } events;

     class_process(uint index)
          : m_index(index)
     {
          m_state = ZERO;
     }

     uint index() { return m_index; }

     void add_event(class_event e)
     {
          events.upcoming.push(e);
     }

     void start_event(uint t)
     {
          events.next().start(t);
          switch ( events.next().type() )
          {
               case class_event::NEW:
               {
                    time.started(events.next().time.request());
                    add_event(class_event(class_event::QUIT, 0));
                    m_state = ZERO;
                    break;
               }
               case class_event::CORE:
               {
                    m_state = RUNNING;
                    break;
               }
               case class_event::DISK:
               {
                    m_state = BLOCKED;
                    break;
               }
               case class_event::DISPLAY:
               {
                    m_state = BLOCKED;
                    break;
               }
               case class_event::INPUT:
               {
                    m_state = BLOCKED;
                    break;
               }
               case class_event::QUIT:
               {
                    time.stopped(t);
                    m_state = TERMINATED;
                    break;
               }
          }
          if ( debug() )
          {
               cout << "TIME " << setw(5) << t << "  " << "process " << index() << "   " << events.next().type_string() << "\t" << "START" << "\t\t" << "for " << events.next().time.request() << endl;
          }
     }
     void stop_event(uint t)
     {
          if ( debug() )
          {
               cout << "TIME " << setw(5) << t << "  " << "process " << index() << "   " << events.next().type_string() << "\t" << "COMPLETE" << "\t" << endl;
          }

          events.complete.push(events.next());
          events.upcoming.pop();
          m_state = READY;
     }

     void terminate(uint t)
     {
          if ( debug() )
          {
               cout << "TIME " << setw(5) << t << "  " << "process " << index() << "   " << events.next().type_string() << endl;
          }

          events.complete.push(events.next());
          events.upcoming.pop();
     }

     void print_upcoming_events()
     {
          queue<class_event> temp = events.upcoming;
          while ( !temp.empty() )
          {
               if ( debug() )
               {
                    cout << "\t\t" << "process " << index() << " event " << temp.front().type_string() << "\t" << "ms " << temp.front().time.request() << endl;
               }
               temp.pop();
          }
     }

private:
     uint m_index;
     enum_state m_state;
};

//		core
//			id
//			process id
//			state (BUSY or IDLE)
struct class_core
{
     enum enum_state
     {
          BUSY,
          IDLE,
     };
     enum_state state() { return m_state; }
     string state_string()
     {
          switch ( m_state )
          {
               case BUSY:
                    return "BUSY";
               case IDLE:
                    return "IDLE";
          }
          return "ERROR";
     }

     struct
     {
          friend class_core;

          uint elapsed() { return m_elapsed; }
          uint started() { return m_started; }
          uint stopped() { return m_stopped; }

          void elapsed(uint t) { m_elapsed = t; }

     private:
          void started(uint t) { m_started = t; }
          void stopped(uint t) { m_stopped = t; }

          uint m_elapsed = 0;
          uint m_started = 0;
          uint m_stopped = 0;
     } time;

     class_core(uint index)
          : m_index(index)
     {
          m_state = IDLE;
     }

     void run(uint t)
     {
          m_state = BUSY;
          time.started(t);
     }
     void end(uint t)
     {
          m_state = IDLE;
          time.stopped(t);
     }

private:
     uint m_index;
     enum_state m_state;
};

//		disk
//			process id
//			state (BUSY or IDLE)
struct class_disk
{
     enum enum_state
     {
          BUSY,
          IDLE,
     };
     enum_state state() { return m_state; }
     string state_string()
     {
          switch ( m_state )
          {
               case BUSY:
                    return "BUSY";
               case IDLE:
                    return "IDLE";
          }
          return "ERROR";
     }

     struct
     {
          friend class_disk;

          uint elapsed() { return m_elapsed; }
          uint started() { return m_started; }
          uint stopped() { return m_stopped; }

          uint elapsed(uint t) { m_elapsed = t; }

     private:
          void started(uint t) { m_started = t; }
          void stopped(uint t) { m_stopped = t; }

          uint m_elapsed = 0;
          uint m_started = 0;
          uint m_stopped = 0;
     } time;

     class_disk(uint index)
          : m_index(index)
     {
          m_state = IDLE;
     }

     void run(uint t)
     {
          m_state = BUSY;
          time.started(t);
     }
     void end(uint t)
     {
          m_state = IDLE;
          time.stopped(t);
     }

private:
     uint m_index;
     enum_state m_state;
};

struct struct_execution
{
     enum enum_type
     {
          BLOCK,
          CORE,
          DISK,
          QUIT
     };
     enum_type type() { return m_type; }

     struct_execution(enum_type type, uint core_id, uint disk_id, uint process_id)
          : m_type(type)
     {
          switch ( type )
          {
               case CORE:
                    m_core = true;
                    break;
               case DISK:
                    m_disk = true;
                    break;
          }

          m_core_id = core_id;
          m_disk_id = disk_id;
          m_process_id = process_id;
     }

     bool core() { return m_core; }
     bool disk() { return m_disk; }

     uint core_id() { return m_core_id; }
     uint disk_id() { return m_disk_id; }
     uint process_id() { return m_process_id; }

     bool prune() { return m_prune; }
     void prune(bool b) { m_prune = b; }

private:
     enum_type & m_type;
     bool m_core = false;
     bool m_disk = false;

     uint m_core_id = 0;
     uint m_disk_id = 0;
     uint m_process_id = 0;

     bool m_prune = false;
};

//		system
//			number of cores
//			core time slice
//			elapsed time
//
//
struct class_system
{
     struct
     {
          friend class_system;

          uint cores() { return m_cores; }
          uint busy_cores() { return m_busy_cores; }
          uint disks() { return m_disks; }
          uint busy_disks() { return m_busy_disks; }
          uint processes() { return m_processes; }

     private:
          void add_core() { ++m_cores; }
          void add_busy_core() { ++m_busy_cores; }
          void remove_busy_core() { --m_busy_cores; }
          void add_disk() { ++m_disks; }
          void add_process() { ++m_processes; }
          void remove_process() { --m_processes; }

          uint m_cores = 0;
          uint m_busy_cores = 0;
          uint m_disks = 0;
          uint m_busy_disks = 0;
          uint m_processes = 0;
     } count;

     struct
     {
          uint core_slice() { return m_core_slice; }
          uint elapsed() { return m_elapsed; }
          uint stopped() { return m_stopped; }
          uint next_start() { return m_next_start; }
          uint next_stop() { return m_next_stop; }

          void core_slice(uint t) { m_core_slice = t; }
          void elapsed(uint t) { m_elapsed = t; }
          void stopped(uint t) { m_stopped = t; }
          void next_start(uint t) { m_next_start = t; }
          void next_stop(uint t) { m_next_stop = t; }

     private:
          uint m_core_slice = 0;
          uint m_elapsed = 0;
          uint m_stopped = 0;
          uint m_next_start = 0;
          uint m_next_stop = 0;
     } time;

     struct
     {
          void processes(vector<class_process> v)
          {
               for ( vector<class_process>::iterator
                       it = v.begin();
                       it != v.end();
                       ++it )
               {
                    cout << "\n" << "Process " << it->index() << " started at " << it->time.started() << " ms and is " << it->state_string();
               }
          }
          void requests(queue<uint> q)
          {
               if ( q.empty() )
               {
                    cout << "\n" << "empty";
               }
               while ( !q.empty() )
               {
                    cout << "\n" << "Process " << q.front();
                    q.pop();
               }
          }
          void details(class_system & sys)
          {
               cout << "\n" << "CURRENT STATE OF THE SYSTEM AT t = " << sys.time.elapsed() << " ms:";
               cout << "\n" << "Current number of busy cores: " << sys.count.busy_cores();
               cout << "\n" << "Current state of the disk unit: " << sys.table_disk.at(0).state_string();

               cout << "\n" << "READY QUEUE:";
               sys.output.requests(sys.queue_core_request);

               cout << "\n" << "DISK QUEUE:";
               sys.output.requests(sys.queue_disk_request);

               cout << "\n" << "PROCESS TABLE:";
               sys.output.processes(sys.table_process);

               cout << "\n" << endl;
               system("pause");
          }
     } output;

     class_process & process(uint index) { return table_process.at(index); }
     class_core    &    core(uint index) { return table_core.at(index); }
     class_disk    &    disk(uint index) { return table_disk.at(index); }

     void add_cores(uint value)
     {
          for ( uint n = 0; n < value; ++n )
          {
               table_core.push_back(class_core(n));
               queue_core.push(n);
               count.add_core();
          }
     }
     void add_disks(uint value)
     {
          for ( uint n = 0; n < value; ++n )
          {
               table_disk.push_back(class_disk(n));
               queue_disk.push(n);
               count.add_disk();
          }
     }
     void add_process()
     {
          uint n = table_process.size();

          table_process.push_back(class_process(n));
          queue_ready.push(n);
          count.add_process();
     }
     void add_process_event(class_event e)
     {
          table_process.back().add_event(e);
     }

     void run()
     {
          bool debug = false;

          if ( debug )
          {
               cout << "SYSTEM START" << endl;
               print_state();
          }

          while ( count.processes() > 0 )
          {
               //static uint iii = 0;
               //cout << "ITERATION " << iii++ << endl;

               // I calculate the time of the next event usin two variables.
               // One variable would suffice, but would require more complexity.
               // Variable time.next_start gets set to system time when events complete.
               // Variable time.next_stop gets the lowest determined stop time of any executing events.
               //      Note: Stop times CAN be determined at the start of an event.
               // The lowest of these is chosen for the next system time.
               // Both variables are set to uint(-1) each iteration.
               //      Note: This value is chosen in hope that it is never reached.
               uint next_time = std::min(time.next_start(), time.next_stop());
               if ( next_time != -1 ) { time.elapsed(next_time); }

               // REQUEST QUEUE
               {
                    // until ready queue is empty
                    time.next_start(-1);
                    while ( !queue_ready.empty() )
                    {
                         class_process & p = process(queue_ready.front());
                         queue_ready.pop();

                         if ( p.events.pending() > 0 )
                         {
                              switch ( p.events.next().type() )
                              {
                                   case class_event::NEW:
                                   {
                                        queue_block.push(p.index());
                                        break;
                                   }
                                   case class_event::CORE:
                                   {
                                        queue_core_request.push(p.index());
                                        break;
                                   }
                                   case class_event::DISK:
                                   {
                                        queue_disk_request.push(p.index());
                                        break;
                                   }
                                   case class_event::DISPLAY:
                                   {
                                        queue_block.push(p.index());
                                        break;
                                   }
                                   case class_event::INPUT:
                                   {
                                        queue_block.push(p.index());
                                        break;
                                   }
                                   case class_event::QUIT:
                                   {
                                        handle_quit(p.index());
                                        break;
                                   }
                              }
                         }
                         else
                         {
                              cout << "ERROR no events but in ready queue" << endl;
                         }
                    }
               }
               if ( debug ) { print_state(); }

               // HANDLE QUEUE
               {
                    handle_requests_blocking();
                    handle_requests_core();
                    handle_requests_disk();
               }
               if ( debug ) { print_state(); }

               // EXECUTION ARRAY
               {
                    time.next_stop(-1);
                    for ( list<struct_execution>::iterator
                            it = list_execution.begin();
                            it != list_execution.end();
                            ++it )
                    {
                         uint process_id = it->process_id();

                         // Check for Completion
                         if ( time.elapsed() == process(process_id).events.next().time.stop_at() )
                         {
                              process(process_id).stop_event(time.elapsed());
                              queue_ready.push(process_id);

                              uint core_id = it->core_id();
                              if ( it->core() )
                              {
                                   core(core_id).end(time.elapsed());
                                   count.remove_busy_core();
                                   queue_core.push(core_id);
                              }

                              uint disk_id = it->disk_id();
                              if ( it->disk() )
                              {
                                   disk(disk_id).end(time.elapsed());
                                   queue_disk.push(disk_id);
                              }

                              it->prune(true);
                              time.next_start(time.elapsed());
                         }
                         else
                         {
                              time.next_stop(std::min(time.next_stop(), process(process_id).events.next().time.stop_at()));
                         }
                    }
                    prune_execution();
               }
               if ( debug ) { print_state(); }

               // TERMINATE QUEUE
               {
                    while ( !queue_terminate.empty() )
                    {
                         uint process_id = queue_terminate.front();
                         queue_terminate.pop();
                         process(process_id).terminate(time.elapsed());
                         count.remove_process();
                         output.details(*this);
                    }
               }
          }
          stop();
     }

     void print_process_table()
     {
          cout << endl;
          for ( vector<class_process>::iterator
                  it = table_process.begin();
                  it != table_process.end();
                  ++it )
          {
               it->print_upcoming_events();
               cout << endl;
          }
     }

     void print_state()
     {
          cout << "\n" << "---------------------";
          cout << "\n" << "|" << "  " << "TIME = " << time.elapsed();


          cout << "\n" << "|";
          cout << "\n" << "|" << "  " << "READY";
          for ( queue<uint> q = queue_ready; !q.empty(); q.pop() )
          {
               cout << "\n" << "|" << "  - " << "Process " << q.front();
          }


          cout << "\n" << "|";
          cout << "\n" << "|" << "  " << "CORE";
          for ( queue<uint> q = queue_core; !q.empty(); q.pop() )
          {
               cout << "\n" << "|" << "  - " << "Core " << q.front();
          }


          cout << "\n" << "|";
          cout << "\n" << "|" << "  " << "CORE REQUEST";
          for ( queue<uint> q = queue_core_request; !q.empty(); q.pop() )
          {
               cout << "\n" << "|" << "  - " << "Process " << q.front();
          }


          cout << "\n" << "|";
          cout << "\n" << "|" << "  " << "DISK";
          for ( queue<uint> q = queue_disk; !q.empty(); q.pop() )
          {
               cout << "\n" << "|" << "  - " << "Disk " << q.front();
          }


          cout << "\n" << "|";
          cout << "\n" << "|" << "  " << "DISK REQUEST";
          for ( queue<uint> q = queue_disk_request; !q.empty(); q.pop() )
          {
               cout << "\n" << "|" << "  - " << "Process " << q.front();
          }


          cout << "\n" << "|";
          cout << "\n" << "|" << "  " << "BLOCK REQUEST";
          for ( queue<uint> q = queue_block; !q.empty(); q.pop() )
          {
               cout << "\n" << "|" << "  - " << "Process " << q.front();
          }


          cout << "\n" << "|";
          cout << "\n" << "|" << "  " << "EXECUTION";
          for ( list<struct_execution>::iterator
                  it = list_execution.begin();
                  it != list_execution.end();
                  ++it )
          {
               cout << "\n" << "|" << "  - " << "Process " << it->process_id();
               if ( it->core() ) { cout << " Core " << it->core_id(); }
               else if ( it->disk() ) { cout << " Disk " << it->disk_id(); }
               else { cout << "       "; }
               cout << " start " << setw(10) << left << process(it->process_id()).events.next().time.started();
               cout << " for " << process(it->process_id()).events.next().time.request() << " ms";
          }


          cout << "\n" << "---------------------";
          cout << endl;
          cout << "\t\t\t\t\t";
          system("pause");
     }

private:

     void stop()
     {
          time.stopped(time.elapsed());
     }

     void handle_quit(uint process_id)
     {
          queue_terminate.push(process_id);
          process(process_id).start_event(time.elapsed());
     }
     void handle_requests_blocking()
     {
          while ( !queue_block.empty() )
          {
               uint process_id = queue_block.front();
               queue_block.pop();
               list_execution.push_back(struct_execution(struct_execution::BLOCK, 0, 0, process_id));
               process(process_id).start_event(time.elapsed());
          }
     }
     void handle_requests_core()
     {
          while ( !queue_core.empty() && !queue_core_request.empty() )
          {
               uint core_id = queue_core.front();
               uint process_id = queue_core_request.front();
               queue_core.pop();
               queue_core_request.pop();
               list_execution.push_back(struct_execution(struct_execution::CORE, core_id, 0, process_id));
               process(process_id).start_event(time.elapsed());
               count.add_busy_core();
          }
     }
     void handle_requests_disk()
     {
          while ( !queue_disk.empty() && !queue_disk_request.empty() )
          {
               uint disk_id = queue_disk.front();
               uint process_id = queue_disk_request.front();
               queue_disk.pop();
               queue_disk_request.pop();
               list_execution.push_back(struct_execution(struct_execution::DISK, 0, disk_id, process_id));
               process(process_id).start_event(time.elapsed());
          }
     }

     void prune_execution()
     {
          for ( list<struct_execution>::iterator
                  it = list_execution.begin();
                  it != list_execution.end();)
          {
               if ( it->prune() )
               {
                    it = list_execution.erase(it);
               }
               else
               {
                    ++it;
               }
          }
     }

     vector<class_process> table_process;
     vector<class_core> table_core;
     vector<class_disk> table_disk;

     queue<uint> queue_ready;
     queue<uint> queue_block;
     queue<uint> queue_core_request;
     queue<uint> queue_core;
     queue<uint> queue_disk_request;
     queue<uint> queue_disk;
     queue<uint> queue_terminate;

     list<struct_execution> list_execution;
} sys;

//		tables
//			process
//			core
//		queues
//			process
//			core
//			disk
//			operation

// STEP 1
//	output
//	1. total simulated time elapsed
//	2. current number of busy cores
//	3. current state of the disk unit (busy/idle)
//	4. contents of CPU queue and disk queue
//	5. for each process
//		a. sequence number
//		b. start time
//		c. current state (READY, RUNNING, BLOCKED, or TERMINATED)
//	when
//		a process is terminated

set<string> valid_tokens
{
     // System Properties
     "NCORES",
     "SLICE",
     // Process Tasks
     "NEW",
     "CORE",
     "DISK",
     "DISPLAY",
     "INPUT"
};
bool is_valid_token(string & s)
{
     return ((ext::set<string>)valid_tokens).contains(s);
}
bool is_valid_line(string & line, string & a, uint & b)
{
     stringstream ss(line);
     ss >> a;
     ss >> b;

     if ( ss.good() )
     {
          //TODO remove before release
          //cout << "extra input exist" << endl;
          // doesn't really matter
     }
     if ( ss.eof() )
     {
          //TODO remove before release
          //cout << "input is valid" << endl;
     }
     if ( ss.fail() )
     {
          //TODO remove output before release
          //cout << "input is invalid" << endl;
          return false;
     }
     if ( !is_valid_token(a) )
     {
          return false;
     }

     return true;
}
// loop through every non-empty line of the input stream
void process_the_stream(ext::set<string> the_valid_tokens)
{
     ifstream file("2.txt");
     if ( !file.is_open() )
     {
          cout << "could not open file" << endl;
          return;
     }

     string line;
     while ( getline(file, line) && line.length() != 0 )
     {
          string token;
          uint value;
          //cout << line << endl;
          if ( is_valid_line(line, token, value) )
          {
               if ( token == "NCORES" )
               {
                    sys.add_cores(value);
                    sys.add_disks(1);
               }
               if ( token == "SLICE" )
               {
                    sys.time.core_slice(value);
               }
               if ( token == "NEW" )
               {
                    sys.add_process();
                    sys.add_process_event(class_event(class_event::NEW, value));
               }
               if ( token == "CORE" )
               {
                    //for (uint n = value / sys.time.core_slice(); n; --n)
                    //{
                    //    value = value - sys.time.core_slice();
                    //    sys.add_process_event(class_event(class_event::CORE, sys.time.core_slice()));
                    //}
                    //if (value > 0)
                    //{
                    sys.add_process_event(class_event(class_event::CORE, value));
                    //}
               }
               if ( token == "DISK" )
               {
                    sys.add_process_event(class_event(class_event::DISK, value));
               }
               if ( token == "DISPLAY" )
               {
                    sys.add_process_event(class_event(class_event::DISPLAY, value));
               }
               if ( token == "INPUT" )
               {
                    sys.add_process_event(class_event(class_event::INPUT, value));
               }
          }
     }
}

int main()
{
     process_the_stream((ext::set<string>)valid_tokens);
     sys.print_process_table();
     sys.run();

     return 0;
}
