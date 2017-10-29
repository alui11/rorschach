/* monitor.h */

#ifndef MONITOR_H
#define MONITOR_H

#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace std;

/* All function prototypes */

void create_map(string path, map<string, pair<bool, time_t> > &files); // Stores initial directory tree into map with file paths and 'is checked?' boolean
vector< vector<string> > read_rules(string rules_file); // Read, parse, check validity of, and store rules
void exec_event(string path, string event, string action); // Execute command from rule via fork/exec/wait
void event_handler(string event, string path, vector< vector<string> > rules); // Match event to rule and dispatch exec_event if applicable
void scan_dir(string path, map<string, pair<bool, time_t> > &files); //Scans directory for CREATE and MODIFY events
void check_deletion(map<string, pair<bool, time_t> > &files); //Checks map for DELETE events
void reset_map(map<string, pair<bool, time_t> > &files); // Resets 'is checked' bools to false

#endif
