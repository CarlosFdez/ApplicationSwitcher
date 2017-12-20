#include <windows.h>
#include <Psapi.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "processes.hpp"
#include "switcher.hpp"
#include "configreader.hpp"
#include "hotkeys.hpp"

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

	ApplicationSwitcher switcher;
	HotkeySystem hotkeys;

	for (SwitcherEntry &entry : entries) {
		switcher.addEntry(entry.name, entry.filter);
		hotkeys.registerKey(entry.hotkey, [&switcher, entry]() {
			cout << "Switching to " << entry.name << endl;
			switcher.switchTo(entry.name);
		});
	}

	hotkeys.processMessages();

	cin.get();

	return 0;
}