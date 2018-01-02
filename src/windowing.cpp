#include "windowing.hpp"

#include <iostream>

using namespace std;

const LPCWSTR windowClass = L"__somecustomwindow__";

constexpr int C_NOTIFICATION_INTERACTION = WM_APP + 1001;

template<typename T>
void EventSystem<T>::addHandler(const EventHandler<T>& evt) {
	handlers.push_back(evt);
}

template<typename T>
void EventSystem<T>::processEvent(const T& evt) {
	for (auto& handler : this->handlers) {
		handler(evt);
	}
}

void WindowsApplication::addMessageHandler(const EventHandler<MSG>& evt) {
	EventSystem<MSG>& system = getSystem();
	system.addHandler(evt);
}

void WindowsApplication::run() {
	EventSystem<MSG>& system = getSystem();

	MSG msg = { 0 };
	while (!stopped && GetMessage(&msg, NULL, 0, 0) != 0) {
		system.processEvent(msg);
	}
}

void WindowsApplication::stop() {
	stopped = true;

	// this makes the ui loop continue so it can do the stopped check 
	PostMessage(NULL, WM_NULL, 0, 0);
}

EventSystem<MSG>& WindowsApplication::getSystem() {
	static EventSystem<MSG> system;
	return system;
}

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
		window->handleEvent(evt);

		if (!evt.isHandled()) {
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}

Window::Window() { }

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

void Window::handleEvent(WindowEvent& evt) {
	if (evt.uMsg == WM_CREATE) {
		this->_hwnd = evt.hwnd;
	}
}

/*
Internal singleton class that registers itself to WindowsApplication.
Extends the application to support menus
*/
class MenuSystem {
public:
	// adds a menu to be managed
	void registerMenu(Menu* item) {
		// todo: mutex
		menus[item->getHandle()] = item;
	}

	// adds a menu to be unmanaged
	void unregisterMenu(Menu* item) {
		if (item->getHandle() != NULL) {
			menus.erase(item->getHandle());
		}
	}

	// todo: create the HMENU here and wrap it in an autodisposal object

	static MenuSystem& getInstance() {
		static MenuSystem system;
		return system;
	}

private:
	MenuSystem() {
		WindowsApplication::addMessageHandler([this](const MSG &evt) {
			if (evt.message != WM_MENUCOMMAND) {
				return false;
			}

			int idx = evt.wParam;
			HMENU menuHandle = (HMENU)evt.lParam;

			if (menus.find(menuHandle) == menus.end()) {
				return false;
			}

			Menu& menu = *menus[menuHandle];
			if (idx < 0 || idx >= menus.size()) {
				// invaliid, we might need to something else
				return true;
			}

			menu[idx].execute();

			return true;
		});
	}

	std::unordered_map<HMENU, Menu*> menus;
};

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

Menu::Menu() {
	info = { 0 };
	info.cbSize = sizeof(MENUINFO);
	info.fMask = MIM_STYLE;
	info.dwStyle = MNS_NOTIFYBYPOS;
}

Menu::~Menu() {
	if (hMenu != NULL) {
		MenuSystem::getInstance().unregisterMenu(this);
		DestroyMenu(hMenu);
	}
}

void Menu::create() {
	if (hMenu != NULL) {
		return; // already created
	}

	hMenu = CreatePopupMenu();
	MenuSystem::getInstance().registerMenu(this);
	SetMenuInfo(hMenu, &(this->info));

	int idx = 0;
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
	int result = TrackPopupMenuEx(hMenu, TPM_VERTICAL | TPM_RIGHTBUTTON, x, y, hwnd, NULL);
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

void NotificationIcon::handleEvent(WindowEvent& evt) {
	Window::handleEvent(evt);

	if (evt.uMsg == WM_CREATE) {
		auto& data = this->data;
		data.hWnd = this->hwnd();
		data.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));

		Shell_NotifyIcon(NIM_ADD, &data);
		Shell_NotifyIcon(NIM_SETVERSION, &data);

		evt.setHandled();
	}
	else if (evt.uMsg == C_NOTIFICATION_INTERACTION) {
		// an action was received
		auto action = LOWORD(evt.lParam);
		if (action == WM_CONTEXTMENU) {
			this->showMenu();
		}

		evt.setHandled();
	}
}

void NotificationIcon::showMenu() {
	if (contextMenu != NULL) {
		POINT pt;
		GetCursorPos(&pt);

		this->contextMenu->showPopup(*this, pt.x, pt.y);
	}

	
}
