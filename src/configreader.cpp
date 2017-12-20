#include "configreader.hpp"

#include <iostream>
#include <optional>

#include "json.hpp"
#include "json_ext.hpp"

using json = nlohmann::json;
using namespace std;

struct SingleMatcherFilters {
	optional<string> className;
	optional<string> filename;
};

/* 
Defines a simple ApplicationFilter, using a set of single value filters.
Returns true if all filters match or no filters were given. Otherwise false.
*/
class SingleMatcher {
public:
	SingleMatcher(SingleMatcherFilters settings) : settings{ settings } {}
	bool operator()(Application test) {
		// if className is defined, we check class name
		auto className = this->settings.className;
		if (className && test.className != className.value()) {
			return false;
		}

		// if filename is defined, we check the last part of the filename
		auto filename = this->settings.filename;
		if (filename) {
			int lastSlash = test.path.find_last_of('/');
			if (lastSlash == string::npos) {
				lastSlash = 0; // we want the whole string
			}
			else {
				lastSlash++; // we want the part after the slash
			}

			string testFile = test.path.substr(lastSlash);
			if (testFile != filename.value()) {
				return false;
			}
		}

		// todo: path/others

		return true;
	}
private:
	SingleMatcherFilters settings;
};

vector<SwitcherEntry> readConfig(const string& filepath) {
	cout << "Reading config file..." << endl;
	ifstream fileReader(filepath);

	if (!fileReader || !fileReader.is_open()) {
		cerr << "Config JSON not found" << endl;
		throw std::runtime_error("Config JSON not found");
	}

	vector<SwitcherEntry> results;

	json document = json::parse(fileReader);
	for (json::value_type& item : document) {
		try {
			SwitcherEntry entry;

			entry.name = item["name"].get<string>();
			cout << "Loading entry " << entry.name << endl;

			entry.hotkey = item["key"].get<string>();

			vector<ApplicationFilter> filterList;
			for (json::value_type& filterProps : item["search"]) {
				SingleMatcherFilters filters;
				filters.className = filterProps["className"].get<optional<string>>();
				filters.filename = filterProps["filename"].get<optional<string>>();

				filterList.push_back(SingleMatcher(filters));
			}

			entry.filter = createUnionFilter(filterList);

			results.push_back(entry);
		}
		catch (std::exception ex) {
			cerr << "Error loading entry: " << ex.what() << endl;
		}
	}

	cout << "entries loaded" << endl << endl;

	return results;
}