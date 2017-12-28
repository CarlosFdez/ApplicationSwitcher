#include "windowing.hpp"

#include <iostream>

using namespace std;

const LPCWSTR windowClass = L"__somecustomwindow__";

constexpr int C_NOTIFICATION_INTERACTION = WM_APP + 1001;



/* Static helper to receive window events. Bind to the window object and send events there. */
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	WindowEvent evt;
	evt.hwnd = hwnd;
	evt.uMsg = uMsg;
	evt.wParam = wParam;
	evt.lParam = lParam;
	
	if (uMsg == WM_CREATE) {
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		Window* window = reinterpret_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		window->handleEvent(evt);
	}
	else {
		auto ptrRaw = GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (!ptrRaw) {
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		Window* window = reinterpret_cast<Window*>(ptrRaw);
		if (!window->handleEvent(evt)) {
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}

Window::Window() {
	Window::onEvent(WM_CREATE, [this](const WindowEvent& evt) {
		// The GWLP_USERDATA was already updated so we just update the hwnd
		this->_hwnd = evt.hwnd;
		return true;
	});
}

Window::~Window() {
	if (created) {
		DestroyWindow(this->hwnd());
	}
}

void Window::create() {
	if (this->created) {
		return;
	}

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

	this->_hwnd = CreateWindowW(windowClass, windowClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr, hInstance, this);

	cout << "DEBUG: Window created" << endl;
	this->created = true;
}

void Window::onEvent(const EventHandler<WindowEvent>& handler) {
	handlers.push_back(handler);
}

void Window::onEvent(int eventType, const EventHandler<WindowEvent>& handler) {
	// todo: optimize using a map or something instead
	Window::onEvent([=](const WindowEvent& evt) {
		if (evt.uMsg == eventType) {
			return handler(evt);
		}
		return false;
	});
}

bool Window::handleEvent(const WindowEvent& evt) {
	bool captured = false;
	for (auto &handler : this->handlers) {
		if (handler(evt)) {
			captured = true;
		}
	}

	return captured;
}

MenuItem::MenuItem(const string& title, const MenuItemCallback& callback) :
		title(title), callback(callback) {
	info = { 0 };
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
	info.fType = MFT_STRING;
}

void MenuItem::create(HMENU hMenu, int idx) {
	info.dwTypeData = const_cast<char *>(this->title.c_str());
	info.wID = idx + 1; // 0 is null data, so we 1-index the list internally
	InsertMenuItem(hMenu, -1, FALSE, &info);
}

Menu::~Menu() {
	if (hMenu != NULL) {
		DestroyMenu(hMenu);
	}
}

void Menu::create() {
	if (hMenu != NULL) {
		return; // already created
	}

	int idx = 0;
	hMenu = CreatePopupMenu();
	for (auto &item : this->items) {
		item.create(hMenu, idx++);
	}
}

void Menu::showPopup(Window &window, int x, int y) {
	HWND hwnd = window.hwnd();
	this->create();

	// todo: this is here to get it to work... make this get removed if this is deleted
	auto items = this->items;

	SetForegroundWindow(hwnd);
	int result = TrackPopupMenuEx(hMenu, TPM_VERTICAL | TPM_RIGHTBUTTON | TPM_RETURNCMD, x, y, hwnd, NULL);
	
	if (result > 0) {
		int idx = result - 1;
		items[idx].execute(); // todo: check for overflow
	}
}

NotificationIcon::NotificationIcon(const string& title) : title(title) {
	data.cbSize = sizeof(data);
	data.uFlags = NIF_SHOWTIP | NIF_MESSAGE | NIF_ICON | NIF_GUID;
	data.uVersion = NOTIFYICON_VERSION_4;
	data.uCallbackMessage = C_NOTIFICATION_INTERACTION;

	GUID guid;
	HRESULT hCreateGuid = CoCreateGuid(&guid); // todo: check if failed?
	data.guidItem = guid;
}

NotificationIcon::~NotificationIcon() {
	Shell_NotifyIcon(NIM_DELETE, &data);
}

void NotificationIcon::create() {
	window.onEvent(WM_CREATE, [this](const WindowEvent& evt) {
		auto& data = this->data;
		data.hWnd = this->window.hwnd();
		data.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));

		Shell_NotifyIcon(NIM_ADD, &data);
		Shell_NotifyIcon(NIM_SETVERSION, &data);

		return true;
	});

	window.onEvent(C_NOTIFICATION_INTERACTION, [this](const WindowEvent& evt) {
		// an action was received
		auto action = LOWORD(evt.lParam);
		if (action == WM_CONTEXTMENU) {
			this->showMenu();
			return true;
		}

		return false;
	});

	window.create();
}

void NotificationIcon::showMenu() {
	if (contextMenu != NULL) {
		POINT pt;
		GetCursorPos(&pt);

		this->contextMenu->showPopup(this->window, pt.x, pt.y);
	}

	
}
