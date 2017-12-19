#include <windows.h>
#include <Psapi.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "processes.hpp"
#include "switcher.hpp"
#include "configreader.hpp"

using namespace std;

/* todo: This is a separate feature of the application. Make it accessible through some flag */ 
void printAll() {
	auto results = getOpenApplications();
	for (auto &entry : results) {
		cout << "Window title: " << entry.title << endl;
		cout << "Class Name: " << entry.className << endl;
		cout << "Executable: " << entry.path << endl;
		cout << endl;
	}
}

int main(void) {
	string filepath = "config.json";
	auto entries = readConfig(filepath);

	ApplicationSwitcher switcher(entries);
	switcher.switchTo("Firefox");

	cin.get();

	return 0;
}