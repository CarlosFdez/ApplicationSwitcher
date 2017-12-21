#include "notificationicon.hpp"

#include <iostream>

using namespace std;

const LPCWSTR windowClass = L"__notificationicon__";
const int CALLBACK_ID = WM_APP + 6495; // hardcoded.......

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {

	if (iMsg == WM_CREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		NotificationIcon* icon = reinterpret_cast<NotificationIcon*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(icon));
		icon->create(hwnd);
	}
	else if (iMsg == CALLBACK_ID) {
		// an action was received
		auto action = LOWORD(lParam);
		if (action == WM_CONTEXTMENU) {
			auto ptrRaw = GetWindowLongPtr(hwnd, GWLP_USERDATA);
			NotificationIcon* icon = reinterpret_cast<NotificationIcon*>(ptrRaw);
			icon->showMenu();
		}
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

NotificationIcon::NotificationIcon(const string& title) : title(title) {
	HINSTANCE hInstance = GetModuleHandle(nullptr);

	static bool classRegistered = false;

	if (!classRegistered) {
		WNDCLASSW cls;
		cls.cbClsExtra = 0;
		cls.cbWndExtra = 0;
		cls.hbrBackground = nullptr;
		cls.hCursor = nullptr;
		cls.hIcon = nullptr;
		cls.hInstance = hInstance;
		cls.lpfnWndProc = WndProc;
		cls.lpszClassName = windowClass;
		cls.lpszMenuName = nullptr;
		cls.style = 0;
		RegisterClassW(&cls);

		classRegistered = true;
	}

	data.cbSize = sizeof(data);
	data.uFlags = NIF_SHOWTIP | NIF_MESSAGE | NIF_ICON | NIF_GUID;
	data.uVersion = NOTIFYICON_VERSION_4;
	data.uCallbackMessage = CALLBACK_ID;

	GUID guid;
	HRESULT hCreateGuid = CoCreateGuid(&guid); // todo: check if failed?
	data.guidItem = guid;

	strcpy(data.szTip, this->title.c_str());

	this->hwnd = CreateWindowW(windowClass, windowClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr, hInstance, this);
}

NotificationIcon::~NotificationIcon()
{
	Shell_NotifyIcon(NIM_DELETE, &data);
}

void NotificationIcon::create(HWND hwnd) {
	this->hwnd = hwnd;

	data.hWnd = this->hwnd;
	data.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));

	Shell_NotifyIcon(NIM_ADD, &data);
	Shell_NotifyIcon(NIM_SETVERSION, &data);
}

void NotificationIcon::showMenu() {
	cout << this->title << endl;
}
