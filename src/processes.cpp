#include "processes.h"

BOOL CALLBACK _processSingleWindow(HWND hwnd, LPARAM lParam);

std::vector<Application> getOpenApplications() {
	// 
	// https://stackoverflow.com/questions/7277366/why-does-enumwindows-return-more-windows-than-i-expected
	
	std::vector<Application> results;
	auto vectorParam = reinterpret_cast<LPARAM>(&results);
	EnumWindows(_processSingleWindow, vectorParam);

	return results;
}

BOOL CALLBACK _processSingleWindow(HWND hwnd, LPARAM lParam) {
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
	GetClassNameA(hwnd, class_name, sizeof(class_name));
	GetWindowTextA(hwnd, title, sizeof(title));

	DWORD processId;
	GetWindowThreadProcessId(hwnd, &processId);

	char filename[MAX_PATH];
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, processId);
	GetProcessImageFileNameA(processHandle, filename, sizeof(filename));
	CloseHandle(processHandle);

	Application result;
	result.className = std::string(class_name);
	result.title = std::string(title);
	result.executable = std::string(filename);

	// todo: does this copy the data?
	results->push_back(result);

	return TRUE;
}
