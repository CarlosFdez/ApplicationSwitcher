#include "processes.hpp"

using namespace std;

BOOL CALLBACK _processSingleWindow(HWND hwnd, LPARAM lParam);
Application getApplicationForWindow(HWND hwnd);

std::vector<Application> getOpenApplications() {
	// 
	// https://stackoverflow.com/questions/7277366/why-does-enumwindows-return-more-windows-than-i-expected
	
	std::vector<Application> results;
	auto vectorParam = reinterpret_cast<LPARAM>(&results);
	EnumWindows(_processSingleWindow, vectorParam);

	return results;
}

optional<Application> getActiveApplication() {
	HWND active = GetActiveWindow();
	if (active) {
		return getApplicationForWindow(active);
	}
	return optional<Application>();
}


Application getApplicationForWindow(HWND hwnd) {
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
	result.windowHandle = hwnd;
	result.className = std::string(class_name);
	result.title = std::string(title);
	result.executable = std::string(filename);

	return result;
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

	auto application = getApplicationForWindow(hwnd);

	// todo: does this copy the data?
	results->push_back(application);

	return TRUE;
}

void Application::toFront() {
	SetForegroundWindow(this->windowHandle);
}
