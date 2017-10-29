/* rorschach.cpp */

#include "monitor.h"

#include <csignal>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <sys/inotify.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <vector>

using namespace std;

vector< vector<string> > rules;
map<string, pair<bool, time_t> > files;
int inotfd, wd;

void usage(int status) {
	cout << "Usage: rorschach [options] ROOT\n\n";
	cout << "Options:\n";
	cout << "\t-h\t\tPrint this help message\n";
	cout <<	"\t-f RULES\tLoad rules from this file (default is rules)\n";
	cout << "\t-t SECONDS\tTime between scans (default is 5 seconds)\n";
	exit(status);
}


void signal_handler_child(int signum) {
	cout << "Cleaning up" << endl;
	cout << "Bye!" << endl;
	exit(0);
}


void signal_handler_parent(int signum) {
	inotify_rm_watch(inotfd, wd);
	close(inotfd);
	exit(0);
}


int main(int argc, char *argv[]) {

	/* default settings */
	const char* RULES_FILE = "rules";
	//float TIME = 5;
	char* PATH;


	/* Parse command line arguments */
	int argind = 1;
	while (argind < argc && strlen(argv[argind]) > 1 && argv[argind][0] == '-') {
		char *arg = argv[argind++];
		switch (arg[1]) {
			case 'h':
				usage(0);
				break;
			case 'f':
				RULES_FILE = argv[argind++];
				break;
			//case 't':
				//TIME = strtof(argv[argind++], NULL);
				//break;
			default:
				usage(1);
				break;
		}
	}
	if (argind >= argc) {
		usage(1);
	} else{
		PATH = argv[argind];			
	}

	/* Read in rules file and store rules */
	rules = read_rules(RULES_FILE);

	/* Show logging message to confirm monitoring directory */
	char abs_path[BUFSIZ];
	realpath(PATH, abs_path);
	cout << "Monitoring " << abs_path << endl;

	/* Perform initial scan and store directory tree */
	create_map(PATH, files);

	/* Create pipe */
	int pipefd[2];
	if (pipe(pipefd)) {
		perror("pipefd");
		return EXIT_FAILURE;
	}

	signal(SIGCHLD, SIG_IGN);

	/* Separate child and parent processes */
	pid_t pid = fork();
	if(pid < 0) {
		cerr << "Unable to fork: " << strerror(errno) << endl;
		exit(1);
	}else if (pid > 0) { //parent
		signal(SIGINT, signal_handler_parent);
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO); // Replace STDOUT w pipe write end

		inotfd = inotify_init();
		if (inotfd < 0){
			cerr << "Couldn't initialize inotify: " << strerror(errno) << endl;
		}
		wd = inotify_add_watch(inotfd, PATH, IN_MODIFY|IN_CREATE|IN_DELETE);
		if (wd < 0){
			cerr << "Couldn't add watch inotify: " << strerror(errno) << endl;
		}

		char buffer[BUFSIZ];
		while (1) {
			int i = 0;
			int length = read(inotfd, buffer, BUFSIZ); 

			if ( length < 0 ) {
				perror( "read" );
			} 

			while (i < length) {
				struct inotify_event *event = ( struct inotify_event * ) &buffer[i];
				if (event->len) {
					if (event->mask & IN_CREATE) {
						cout << "CREATE " << event->name << endl;
					}
					if ( event->mask & IN_MODIFY) {
							cout << "MODIFY " << event->name << endl;
					}
					if ( event->mask & IN_DELETE) {
						cout << "DELETE " << event->name << endl;
					} 

					i += (sizeof(struct inotify_event)) + event->len;
				}
			}
		}

		/*
		// Scan directory every TIME seconds and report detected events
		while(true){
		usleep(TIME * 1e6);
		scan_dir(PATH, files); // Check CREATE/MODIFY
		check_deletion(files); // Check DELETE
		reset_map(files); // Reset to check again
		}
		*/

	}else{ //child
		/* catch ^C */
		signal(SIGINT, signal_handler_child);

		close(pipefd[1]);
		dup2(pipefd[0], STDIN_FILENO);

		string event;
		string full_path;
		while(true){
			cin >> event >> full_path;

			cout << "Detected \"" << event << "\" event on \"" << full_path << "\"" << endl;

			event_handler(event, full_path, rules);
		}
	}

}
