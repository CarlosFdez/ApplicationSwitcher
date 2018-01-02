#pragma once
// solution above from https://social.msdn.microsoft.com/Forums/vstudio/en-US/722193a4-7fba-4ed9-a41f-6629efdb2a78/loadiconmetric-corresponding-ordinal-number-380-could-not-be-located?forum=vcgeneral

#include <windows.h>
#include <WinUser.h>
#include <Commctrl.h>
#include <string>
#include <functional>
#include <optional>


/*
Defines a function that processes an arbitrary object.
Returns true if the message was handled.
*/
template<typename T>
using EventHandler = std::function<bool(const T& evt)>;

/* Defines an object that pushes events to registered handlers */
template<typename T>
class EventSystem {
public:
	// todo: design an implementation for a remove handler mechanism

	void addHandler(const EventHandler<T>& evt);

	virtual void processEvent(const T& evt);

private:
	std::list<EventHandler<T>> handlers;

	friend class EventResource;
};

/*
Defines a windows application.
Runs and controls the global application event loop.
*/
class WindowsApplication {
public:
	/*
	Adds a message handler to the message loop.
	Use it to add new features to the loop.
	*/
	static void addMessageHandler(const EventHandler<MSG>& evt);

	static void run();
	static void stop();

private:
	static EventSystem<MSG>& getSystem();

	inline static bool stopped = false;

	friend class Menu;
};

struct WindowEvent {
	/* The window handle that generated the event */
	HWND hwnd;

	/* The type of event that was generated*/
	UINT uMsg;

	WPARAM wParam;
	LPARAM lParam;

	/* Flags this event as handled. Unhandled events go to DefWndProc. */
	void setHandled() {
		_isHandled = true;
	}

	/* Checks if this event as handled. Unhandled events go to DefWndProc. */
	bool isHandled() {
		return _isHandled;
	}

private:
	bool _isHandled = false;
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
	virtual void create();

	/* Handles the triggered window event. Override to add new functionality */
	virtual void handleEvent(WindowEvent& evt);

private:
	bool created = false;
	HWND _hwnd;


	std::list<EventHandler<WindowEvent>> handlers;
};

using MenuItemCallback = std::function<void(void)>;

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
	Menu();
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

	size_t size() {
		return items.size();
	}

	MenuItem& operator[](int idx) {
		return items[idx];
	}

protected:
	void create();

private:
	std::vector<MenuItem> items;

	HMENU hMenu = NULL;
	MENUINFO info;
};


/*
Represents a Win32 notification icon (system tray icon).
*/
class NotificationIcon : public Window {
public:
	NotificationIcon(const std::string& title);
	~NotificationIcon();

	void showMenu();

	void setContextMenu(Menu* menu) {
		this->contextMenu = menu;
	}

	/* Handles the triggered window event. Override to add new functionality */
	virtual void handleEvent(WindowEvent& evt);

private:
	std::string title;

	Menu* contextMenu = NULL;

	// the notification icon data
	NOTIFYICONDATA data;
};