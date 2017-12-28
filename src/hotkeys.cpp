#include "hotkeys.hpp"

#include <algorithm>
#include <string>
#include "stringext.hpp"

using namespace std;

void HotkeySystem::registerKey(const Key& key, KeyCallback onUse) {
	int id = ++this->currentId;
	RegisterHotKey(NULL, id, key.modifiers, key.virtualKey);
	bindingMap[id] = onUse;
}

void HotkeySystem::registerKey(const string& key, KeyCallback onUse) {
	this->registerKey(parseKey(key), onUse);
}

void HotkeySystem::processMessages() {
	MSG msg = { 0 };
	while (!stopped && GetMessage(&msg, NULL, 0, 0) != 0) {
		this->processMessage(msg);
	}
}


void HotkeySystem::processMessage(const MSG& msg) {
	if (msg.message != WM_HOTKEY) {
		return;
	}

	int hotkeyId = msg.wParam;
	if (bindingMap.find(hotkeyId) != bindingMap.end()) {
		// execute registered callback function
		this->bindingMap[hotkeyId]();
	}
}

void HotkeySystem::stop() {
	stopped = true;

	// this makes the ui loop continue so it can do the stopped check 
	PostMessage(NULL, WM_NULL, 0, 0);
}

// todo: create some bimap structure to go both ways allowing repeats
static map<string, UINT> keycodesMap = {
	{ "backspace", VK_BACK },
	{ "tab", VK_TAB },
	{ "enter", VK_RETURN },

	{ "shift", VK_SHIFT },
	{ "lshift", VK_LSHIFT },
	{ "rshift", VK_RSHIFT },

	{ "ctrl", VK_CONTROL },
	{ "lctrl", VK_LCONTROL },
	{ "rctrl", VK_RCONTROL },

	{ "alt", VK_MENU },
	{ "lalt", VK_LMENU },
	{ "ralt", VK_RMENU },

	{ "space", VK_SPACE },
	{ "pageup", VK_PRIOR },
	{ "pagedown", VK_NEXT },
	{ "end", VK_END },
	{ "home", VK_HOME },

	{ "left", VK_LEFT },
	{ "up", VK_UP },
	{ "right", VK_RIGHT },
	{ "down", VK_DOWN }
};

/* 
Parses the given key name to return a virtual keycode.
todo: move to its own file once this is more fleshed out.
*/
UINT parseKeyCode(string keyString) {
	// keycodes https://msdn.microsoft.com/en-us/library/dd375731(v=vs.85).aspx

	keyString = ext::trim(keyString);
	ext::lower(keyString);
	
	if (keycodesMap.find(keyString) != keycodesMap.end()) {
		return keycodesMap[keyString];
	}

	// todo: numpad digits

	// from now on, we're checking single character keys
	// any bigger and its a failure
	if (keyString.size() > 1) {
		return 0;
	}

	char keychar = keyString[0];

	if (isdigit(keychar)) {
		int idx = keychar - '0';
		return 0x30 + idx;
	}

	if (isalpha(keychar)) {
		int idx = keychar - 'a';
		return 0x41 + idx;
	}

	return 0;
}

Key parseKey(string key) {
	Key result;

	// trim and to lower case
	key = ext::trim(key);
	transform(key.begin(), key.end(), key.begin(), ::tolower);

	if (key.empty()) {
		return result; // todo exception instead
	}

	// if the last character is +, replace it for "plus".
	// + is used as a delimeter so not replacing can cause problems.
	if (key[key.size() - 1] == '+') {
		key = key.substr(0, key.size() - 1) + "plus";
	}

	// split into parts, which are modifiers followed by the key
	vector<string> parts = ext::split(key, '+');
	
	string finalKey = parts[parts.size() - 1];
	parts.pop_back();

	for (string &modifierStr : parts) {
		modifierStr = ext::trim(modifierStr);
		if (modifierStr.empty()) {
			continue;
		}

		if (modifierStr == "ctrl") {
			result.modifiers |= MOD_CONTROL;
		}
		else if (modifierStr == "alt") {
			result.modifiers |= MOD_ALT;
		}
		else if (modifierStr == "shift") {
			result.modifiers |= MOD_SHIFT;
		}
		// todo else throw exception invalid modifier
	}

	// todo: throw if zero
	result.virtualKey = parseKeyCode(finalKey);

	return result;
}