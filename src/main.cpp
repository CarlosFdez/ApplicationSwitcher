#include <windows.h>
#include <Psapi.h>
#include <iostream>

#include "processes.h"

using namespace std;

/* todo: This is a separate feature of the application. Make it accessible through some flag */ 
void printAll() {
	auto results = getOpenApplications();
	for (auto &entry : results) {
		cout << "Window title: " << entry.title << endl;
		cout << "Class Name: " << entry.className << endl;
		cout << "Executable: " << entry.executable << endl;
		cout << endl;
	}
}

int main(void) {
	printAll();

	int x;
	cin >> x;
}