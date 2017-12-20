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
		auto className = this->settings.className;
		if (className && test.className != className.value()) {
			return false;
		}

		// todo: executable/filename/others

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