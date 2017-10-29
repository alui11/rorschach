/* map.cpp */

#include "monitor.h"

#include <cstdlib>
#include <cerrno>
#include <dirent.h>
#include <map>
#include <iostream>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utility>

using namespace std;


/*
 * Scans the directory for the first time and stores as map of absolute
 * paths and 'is checked' booleans (initially false - used when checking
 * for deletions later)
 */
void create_map(string path, map<string, pair<bool, time_t> > &files) {
	DIR *d = opendir(path.c_str());
	if (d == NULL) {
		cerr << "Unable to opendir: " << path << " " << strerror(errno) << endl; 
		exit(1);
	}       
	struct dirent *e;
	while ((e = readdir(d))) {
		if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) continue;        
		struct stat s;
		string full_path = path + "/" + e->d_name;
		if (stat(full_path.c_str(), &s) == 0) {
			if (S_ISREG(s.st_mode)) {
				pair<bool, time_t> val (false, s.st_mtime);
				files.insert(pair<string, pair<bool, time_t> >(full_path, val));
			} else if (S_ISDIR(s.st_mode)) {
				create_map(full_path, files);
			}       
		}       
	}       
	closedir(d);
}


/*
 * Resets map's 'is checked' booleans to false after finishing scan for
 * DELETE events
 */
void reset_map(map<string, pair<bool, time_t> > &files) {
	for (auto it = files.begin(); it != files.end(); it++) {
		it->second.first = false;
	}   
}
