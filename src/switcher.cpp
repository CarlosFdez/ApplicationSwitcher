#include "switcher.hpp"

#include <algorithm>
#include <vector>
#include <string>

using namespace std;

ApplicationSwitcher::ApplicationSwitcher(const vector<SwitcherEntry> &entries) {
	for (auto entry : entries) {
		this->entries.push_back(entry);

		this->entryByName[entry.name] = entry;
	}
}

void ApplicationSwitcher::switchTo(const string &name) {
	if (entryByName.find(name) == entryByName.end()) {
		throw new std::runtime_error("Entry does not exist");
	}

	auto entry = entryByName[name];

	auto applications = getOpenApplications();

	for (auto &app : applications) {
		if (!entry.matches(app)) {
			continue;
		}

		app.toFront();
		break;
	}
}
