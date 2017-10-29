/* event_detector.cpp */

#include "monitor.h"

#include <cstring>
#include <dirent.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>


/* 
 * Scan the directory recursively and check for CREATE or MODIFY events
 * that occurred after last_scan time
 */
void scan_dir(string path, map<string, pair<bool, time_t> > &files) {
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
				/* Check whether file already exists in stored map */
				auto it = files.find(full_path);
				if (it != files.end())  {
					it->second.first = true;
					/* check last modification time */
					if (s.st_mtime > it->second.second) {
						//cout << "Detected \"MODIFY\" event on \"" << full_path << "\"" << endl;
						cout << "MODIFY " << full_path << endl;
						it->second.second = s.st_mtime;
						//event_handler("MODIFY", full_path, rules);
					}   
				} else {
					//cout << "Detected \"CREATE\" event on \"" << full_path << "\"" << endl;
					cout << "CREATE " << full_path << endl;
					pair<bool, time_t> val (true, s.st_mtime);
					files.insert(pair<string, pair<bool, time_t> >(full_path, val));
					//event_handler("CREATE", full_path, rules);
				}
			} else if (S_ISDIR(s.st_mode)) {
				scan_dir(full_path, files);
			}
		}
	}
	closedir(d);
}


/* Check for DELETE events */
void check_deletion(map<string, pair<bool, time_t> > &files) {
	vector< map< string, pair<bool, time_t> >::iterator > to_delete;
	for (auto it = files.begin(); it != files.end(); it++) {
		if (it->second.first == false) {
			//cout << "Detected \"DELETE\" event on \"" << it->first << "\"" << endl;
			cout << "DELETE " << it->first << endl;
			//event_handler("DELETE", it->first, rules);
			to_delete.push_back(it);
		}   
	}   
	for (auto it:to_delete){
		files.erase(it);
	}
}
