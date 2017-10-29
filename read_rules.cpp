/* read_rules.cpp */

#include "monitor.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>


/*
 * Read rules file, parse, check validity, and store rules as
 * table (nested vectors)
 */
vector< vector<string> > read_rules(string rules_file) {
	vector< vector<string> > stored_rules;
	ifstream rules;
	rules.open(rules_file);
	while (!rules.eof()) {
		string line;
		getline(rules, line);
		if (line.empty() || line[0] == '#') {
			continue;
		}   
		string full_line = line;
		size_t pos = 0;
		if((pos = line.find_first_of(" \t")) == size_t(-1)) {
			cerr << "Invalid rule: " << full_line << endl;
			exit(1);
		}   
		string event = line.substr(0, pos);
		if (event != "MODIFY" && event != "CREATE" && event != "DELETE") {
			cerr << "Invalid rule: " << full_line << endl;
			exit(1);
		}   
		line.erase(0, pos);
		if ((pos = line.find_first_not_of(" \t")) == size_t(-1)) {
			cerr << "Invalid rule: " << full_line << endl;
			exit(1);
		}   
		line.erase(0, pos);
		if ((pos = line.find_first_of(" \t")) == size_t(-1)) {
			cerr << "Invalid rule: " << full_line << endl;
			exit(1);
		}   
		string pattern = line.substr(0, pos);
		line.erase(0, pos);
		if ((pos = line.find_first_not_of(" \t")) == size_t(-1)) {
			cerr << "Invalid rule: " << full_line << endl;
			exit(1);
		}   
		line.erase(0, pos);
		string action = line;
		vector<string> rule {event, pattern, action};
		stored_rules.push_back(rule);
	}   
	rules.close();
	return stored_rules;
}
