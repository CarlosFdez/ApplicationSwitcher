#pragma once

#include <vector>
#include <map>
#include <string>
#include <functional>

#include "processes.hpp"

using ApplicationFilter = std::function<bool(Application)>;

/* Creates a new application filter based on a list of application filters */
ApplicationFilter createUnionFilter(const std::vector<ApplicationFilter>& filters);

/*
Defines a structure used to define the behavior of the ApplicationSwitcher
for a single application. Currently its just a name and a check function.
*/
struct SwitcherEntry {
	std::string name;
	ApplicationFilter filter;

	inline bool matches(Application app) {
		return this->filter(app);
	}
};

/*
Defines a class used to control switching to an application. 
This class needs to be used manually using the switchTo function.
*/
class ApplicationSwitcher {
public:
	ApplicationSwitcher(const std::vector<SwitcherEntry>& entries);
	ApplicationSwitcher(const ApplicationSwitcher& other) = default;
	ApplicationSwitcher(ApplicationSwitcher&& other) = default;

	void switchTo(const std::string& name);

private:
	std::vector<SwitcherEntry> entries;
	std::map<std::string, SwitcherEntry> entryByName;
};