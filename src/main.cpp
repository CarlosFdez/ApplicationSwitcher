#include <windows.h>
#include <Psapi.h>

#include <iostream>
#include <string>
#include <vector>

struct Application {
	std::string title;
	std::string className;
	std::string executable;
};

BOOL CALLBACK _processSingleWindow(HWND hwnd, LPARAM lParam);

std::vector<Application> getOpenApplications() {
	// https://stackoverflow.com/questions/7277366/why-does-enumwindows-return-more-windows-than-i-expected
	std::vector<Application> results;
	auto vectorParam = reinterpret_cast<LPARAM>(&results);
	EnumWindows(_processSingleWindow, vectorParam);

	return results;
}

BOOL CALLBACK _processSingleWindow(HWND hwnd, LPARAM lParam)
{
	auto results = reinterpret_cast<std::vector<Application>*>(lParam);

	if (!IsWindowVisible(hwnd)) {
		return TRUE;
	}

	// Tool windows should not be displayed either, these do not appear in the
	// task bar.
	if (GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) {
		return TRUE;
	}

	char class_name[80];
	char title[80];
	GetClassName(hwnd, class_name, sizeof(class_name));
	GetWindowText(hwnd, title, sizeof(title));

	DWORD processId;
	char filename[MAX_PATH];
	GetWindowThreadProcessId(hwnd, &processId);
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, processId);
	GetProcessImageFileName(processHandle, filename, sizeof(filename));
	CloseHandle(processHandle); // todo: RAII


	Application result;
	result.className = std::string(class_name);
	result.title = std::string(title);
	result.executable = std::string(filename);

	// todo: does this copy the data?
	results->push_back(result);

	return TRUE;
}

int main(void) {
	auto results = getOpenApplications();
	for (auto &entry : results) {
		std::cout << "Window title: " << entry.title << std::endl;
		std::cout << "Class Name: " << entry.className << std::endl;
		std::cout << "Executable: " << entry.executable << std::endl;
		std::cout << std::endl;
	}
}