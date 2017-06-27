#include "stdafx.h"

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
     enum TOKEN { NCORES, NEW, CORE, DISK, DISPLAY, INPUT, NONE };

     // need a structure to hold a rows worth of data
     // { id    request    value    blocking    start_time    end_time }
     struct
          row
     {
          int id;    TOKEN request;    int value;     bool blocking;    int start_time;    int end_time;
     };

     // make a table to hold history of events
     class
          table
     {
     public:
          table(vector<row> && rows) :rows(rows) {}

          void print()
          {
               for ( auto&& r : rows )
               {
                    cout << setw(3) << r.id << " " << setw(10) << left << TOKEN_STRING[r.request] << " " << setw(5) << right << r.value << " " << setw(5) << r.start_time << " " << setw(5) << r.end_time << " " << r.blocking << endl;
               }
               cout << endl;
          }
     private:
          vector<row> rows;
     };

     // input file
     // {NEW,  t} creates new process at time t
     // {CORE, t} executes instructions for duration t
     // {DISK, 0} blocks disk access for 10ms
     // {DISK, 1} blocks disk access and process for 10ms
     // {DISPLAY, t} blocks process for duration t
     // {INPUT, t} blocks process for duration t

     /// TODO: do this later
     //table
     //     build_from_stream
     //     (
     //          istream & input_stream
     //     )
     //{
     //     table new_table;
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
     //          row new_row;
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
          builder_row
     {
     public:
          builder_row() = default;
          builder_row(const builder_row&) = delete;
          builder_row& operator=(const builder_row&) = delete;

          row
               row_via_jump_table
               (
                    tuple<string const, int const> const & pair
               )
          {
               s = get<0>(pair);         // token
               r.value = get<1>(pair);   // value
               char const & c = s.at(0); // first character of token

               // hash function: c % 4
               // used as index into jump table
               (this->*jump[(c % 4)])();

               return r;
          }

     private:
          row r = { -1, NONE, true, 0, 0, 0 };
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
          void(builder_row::* jump[4])() = { &builder_row::jump_D, &builder_row::jump_I, &builder_row::jump_N, &builder_row::jump_C };
     };

     class
          builder_table
     {
     public:
          static vector<row>
               build_via_jump_table
               (
                    istream & input_stream
               )
          {
               vector<row> rows;

               builder_row br;
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
                    vector<row> & rows
               )
          {
               int prev_pid = 0;
               int prev_end = 0;

               auto && beg = rows.begin();
               auto && end = rows.end();
               for ( ; beg != end; ++beg )
               {
                    auto && r = *beg;

                    if ( r.id == prev_pid )
                    {
                         r.start_time = prev_end;
                    }
                    else {
                         prev_pid = r.id;
                    }
                    prev_end = r.end_time = r.start_time + r.value;
               }
          }
     };

     table build_from_stream(istream & input_stream)
     {
          return builder_table::build_via_jump_table(input_stream);
     }
};

int main(int argc, const char ** argv)
{
     using namespace EVENT_TABLE;

     table t = build_from_stream(ifstream("1.txt"));
     t.print();

     return 0;
}
