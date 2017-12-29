#pragma once

#include <windows.h>
#include <functional>
#include <map>

struct Key {
	UINT modifiers = 0;
	UINT virtualKey = 0;
};

/* Represents a callback executed when the hotkey is pressed */
using KeyCallback = std::function<void()>;

/*
Represents a container used to register and process hotkeys
*/
class HotkeySystem {
public:
	void registerKey(const Key& key, KeyCallback onUse);

	/* 
	Registers a callback to the given key string.
	The key string is parsed by the "parseKey" function.
	*/
	void registerKey(const std::string& key, KeyCallback onUse);

	void processMessages();
	bool processMessage(const MSG& msg);

	void stop();

private:
	int currentId = 0;
	std::map<int, KeyCallback> bindingMap;

	bool stopped = false;
};

/* returns the key code of a single key without modifiers */
UINT parseKeyCode(std::string keyString);

/* parses an entire keycode string including modifiers */
Key parseKey(std::string key);
