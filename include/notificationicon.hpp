#pragma once
// solution above from https://social.msdn.microsoft.com/Forums/vstudio/en-US/722193a4-7fba-4ed9-a41f-6629efdb2a78/loadiconmetric-corresponding-ordinal-number-380-could-not-be-located?forum=vcgeneral

#include <windows.h>
#include <WinUser.h>
#include <Commctrl.h>
#include <string>

class NotificationIcon {
public:
	NotificationIcon(const std::string& title);
	~NotificationIcon();

	void create(HWND hwnd);
	void showMenu();

private:
	HWND hwnd;
	std::string title;

	// the notification icon data
	NOTIFYICONDATA data;
};
