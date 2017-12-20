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
Defines a class used to control switching to an application. 
This class needs to be used manually using the switchTo function.
*/
class ApplicationSwitcher {
public:
	void addEntry(const std::string& name, const ApplicationFilter& filter);

	void switchTo(const std::string& name);

private:
	std::map<std::string, ApplicationFilter> entryByName;
};