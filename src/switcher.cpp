#include "switcher.hpp"

#include <algorithm>
#include <vector>
#include <string>

using namespace std;

ApplicationFilter createUnionFilter(const vector<ApplicationFilter>& filters) {
	if (filters.empty()) {
		return [](Application app) { return false; };
	}
	else {
		// return a "match at least once". Copy the list internally
		return [=](Application test) {
			for (auto& filter : filters) {
				if (filter(test)) {
					return true;
				}
			}
			return false;
		};
	}
}

void ApplicationSwitcher::addEntry(const string& name, const ApplicationFilter& filter) {
	this->entryByName[name] = filter;
}

void ApplicationSwitcher::switchTo(const string& name) {
	if (entryByName.find(name) == entryByName.end()) {
		throw new std::runtime_error("Entry does not exist");
	}

	ApplicationFilter& filter = entryByName[name];

	auto applications = getOpenApplications();

	for (Application &app : applications) {
		if (!filter(app)) {
			continue;
		}

		app.toFront();
		break;
	}
}
