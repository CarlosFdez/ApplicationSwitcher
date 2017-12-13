#pragma once

#include <windows.h>
#include <Psapi.h>

#include <string>
#include <vector>

/*
Represents an open application with a top level visible window
*/
struct Application {
	std::string title;
	std::string className;
	std::string executable;
};

/*
Returns all top level visible windows.
 */
std::vector<Application> getOpenApplications();