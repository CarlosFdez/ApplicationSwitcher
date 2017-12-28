#include <windows.h>
#include <Psapi.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "processes.hpp"
#include "switcher.hpp"
#include "configreader.hpp"
#include "hotkeys.hpp"

#include "windowing.hpp"

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

int main() {
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
	
	NotificationIcon icon("Test");


	Menu notificationMenu;
	notificationMenu.addItem("Exit", [&]() { 
		hotkeys.stop();
	});

	icon.setContextMenu(&notificationMenu);
	icon.create();

	hotkeys.processMessages();

	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR     lpCmdLine,
					 int       nCmdShow) {
	return main();
}