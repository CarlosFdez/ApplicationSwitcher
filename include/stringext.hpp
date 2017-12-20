#pragma once

/*
Defines additional functions to work on strings
that are missing from the standard library.
*/

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

namespace ext {

	/* Destructively sets a string to lowercase */
	void lowerD(std::string& str) {
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	}

	/* Creates a new string with the contents of the original, but lowercase */
	std::string lower(std::string str) {
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}

	/* Trims the right side of a string with no side effects. */
	std::string trimRight(const std::string& str) {
		size_t end = str.find_last_not_of(' ');
		return str.substr(0, end + 1);
	}

	/* Creates a new string with both sides trimmed of space characters. */
	std::string trim(const std::string& str) {
		size_t start = str.find_first_not_of(' ');
		size_t end = str.find_last_not_of(' ');
		return str.substr(start, end - start + 1);
	}

	/* Splits a string into parts, preserving the original */
	std::vector<std::string> split(const std::string& str, const char& delimeter) {
		std::vector<std::string> results;

		std::stringstream stream(str);
		std::string item;
		while (getline(stream, item, delimeter)) {
			results.push_back(item);
		}

		return results;
	}

}
