#pragma once
// solution above from https://social.msdn.microsoft.com/Forums/vstudio/en-US/722193a4-7fba-4ed9-a41f-6629efdb2a78/loadiconmetric-corresponding-ordinal-number-380-could-not-be-located?forum=vcgeneral

#include <windows.h>
#include <WinUser.h>
#include <Commctrl.h>
#include <string>
#include <functional>
#include <optional>

using MenuItemCallback = std::function<void(void)>;

template<typename T>
using EventHandler = std::function<bool(const T& evt)>;

struct WindowEvent {
	HWND hwnd;
	UINT uMsg;
	WPARAM wParam;
	LPARAM lParam;
};

/*
A simple wrapper over the win32 window. 
Manages window events and passes them on to registered event handlers.
*/
class Window {
public:
	Window();
	Window(const Window& other) = delete;
	Window(Window&& other) = default;
	~Window();

	HWND hwnd() const { return this->_hwnd; }
	
	/** Creates new window, allocating the required resources */
	void create();

	void onEvent(const EventHandler<WindowEvent>& handler);
	void onEvent(int eventType, const EventHandler<WindowEvent>& handler);

	bool handleEvent(const WindowEvent& evt);

private:
	bool created = false;
	HWND _hwnd;


	std::list<EventHandler<WindowEvent>> handlers;
};


class MenuItem {
public:
	MenuItem(const std::string& title, const MenuItemCallback& callback);

	void execute() {
		callback();
	}

	const std::string& getTitle() const {
		return title;
	}

protected:
	/* Internal method to build the actual menu item */
	void create(HMENU menu, int idx);

private:
	std::string title;
	MenuItemCallback callback;

	MENUITEMINFO info = { 0 };


	friend class Menu;
};

/* 
Represents a Menu. Menus must be associated with a window to process events.
*/
class Menu {
public:
	/* Creates a new menu associated with the given window */
	Menu() {}
	Menu(const Menu& other) = delete;
	Menu(Menu&& other) = default;
	~Menu();

	/* Returns the menu handle if it was created. Otherwise returns NULL */
	HMENU getHandle() {
		return hMenu;
	}

	void addItem(const std::string& title, MenuItemCallback callback) {
		items.emplace_back(title, callback);
	}

	void showPopup(Window &window, int x, int y);

protected:
	void create();

private:
	Window * window;
	std::vector<MenuItem> items;

	HMENU hMenu = NULL;
};


/*
Represents a Win32 notification icon (system tray icon).
*/
class NotificationIcon {
public:
	NotificationIcon(const std::string& title);
	~NotificationIcon();

	void create();
	void showMenu();

	void setContextMenu(Menu* menu) {
		this->contextMenu = menu;
	}

private:
	Window window;
	std::string title;

	Menu* contextMenu = NULL;

	// the notification icon data
	NOTIFYICONDATA data;
};