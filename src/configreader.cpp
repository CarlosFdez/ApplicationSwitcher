#include "configreader.hpp"

#include <iostream>
#include "json.hpp"
#include <optional>

using json = nlohmann::json;
using namespace std;


bool nullfilter(Application app) {
	return false;
}

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

// todo: move somewhere else as a set of json extensions.
namespace std {
	template <typename T>
	void to_json(json& j, const optional<T>& a) {
		if (a.has_value()) {
			j = a.value();
		}
	}

	template <typename T>
	void from_json(const json& j, optional<T>& a) {
		if (j.is_null()) {
			a = optional<T>();
		}
		else {
			a = optional<T>(j.get<T>());
		}
	}
}

/* Defines an ApplicationFilter that succeeds if any of the given filters succeeds. */
class MatchAny {
public:
	MatchAny(vector<ApplicationFilter> filters) {
		this->filters = filters;
	}

	bool operator()(Application test) {
		for (auto &filter : this->filters) {
			if (filter(test)) {
				return true;
			}
		}
		return false;
	}

private:
	vector<ApplicationFilter> filters;
};

vector<SwitcherEntry> readConfig(const string& filepath)
{
	cout << "Reading config file..." << endl;
	ifstream fileReader(filepath);

	if (!fileReader || !fileReader.is_open()) {
		cerr << "Config JSON not found" << endl;
		throw std::runtime_error("Config JSON not found");
	}

	vector<SwitcherEntry> results;

	json document = json::parse(fileReader);
	for (auto &item : document) {
		SwitcherEntry entry;

		entry.name = item.at("name").get<string>();
		cout << "Got entry " << entry.name << endl;

		// todo: Seperate the filter generation somehow. 
		// Perhaps this should return config entries and something else transforms them into Switcher entries?

		vector<ApplicationFilter> filterList;
		for (auto filterProps : item["search"]) {
			SingleMatcherFilters filters;
			filters.className = filterProps["className"].get<optional<string>>();
			filters.filename = filterProps["filename"].get<optional<string>>();

			filterList.push_back(SingleMatcher(filters));
		}

		// now assign the correct filter to the entry
		if (filterList.size() == 0) {
			entry.filter = nullfilter;
		}
		else if (filterList.size() == 1) {
			entry.filter = filterList[0];
		}
		else {
			entry.filter = MatchAny(filterList);
		}

		results.push_back(entry);
	}

	return results;
}