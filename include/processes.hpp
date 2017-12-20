#pragma once

#include <windows.h>
#include <Psapi.h>

#include <string>
#include <vector>
#include <optional>

/*
Represents an open application with a top level visible window
*/
struct Application {
	HWND windowHandle;
	std::string title;
	std::string className;

	/* The path of the executable running the application. 
	All path delimiters are replaced with / for consistency. */
	std::string path;

	/* Moves this application to the front using the window handle */
	void toFront();
};

/*
Returns all top level visible windows.
 */
std::vector<Application> getOpenApplications();

/* Return the current active application if there is one. */
std::optional<Application> getActiveApplication();