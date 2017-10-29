/* event_handler.cpp */

#include "monitor.h"

#include <cstring>
#include <fnmatch.h>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>


/*
 * Check if event matches rule, dispatch execute function if match found
 */
void event_handler(string event, string path, vector< vector<string> > rules) {
	for (auto rule:rules) {
		if (event == rule[0]) {
			if (fnmatch(rule[1].c_str(), basename(path.c_str()), 0) == 0) {
				cout << "Matched \"" << rule[1] << "\" pattern on \"" << basename(path.c_str()) << "\"" << endl;
				exec_event(path, event, rule[2]);
			}
		}           
	}           
}


/* Execute specified command via forking */
void exec_event(string path, string event, string action) {

	/* Set environment variables */
	setenv("BASEPATH", basename(path.c_str()), 1);
	setenv("FULLPATH", path.c_str(), 1);
	setenv("EVENT", event.c_str(), 1);
	setenv("TIMESTAMP", to_string(time(0)).c_str(), 1);

	/* fork and exec */
	pid_t pid = fork();
	if (pid < 0) {
		cerr << "Unable to fork:" << strerror(errno) << endl;
		exit(1);
	} else if (pid == 0) {
		cout << "Executing action \"" << action << "\" on \"" << basename(path.c_str()) << "\"" << endl;
		if (execlp("/bin/sh", "/bin/sh", "-c", action.c_str(), NULL) != 0) {
			cerr << "Unable to exec: " << strerror(errno) << endl;
			exit(1); 
		}       
	} else {
		int status;
		while ((pid = wait(&status)) < 0);
	}       
}
