#pragma once

#include <optional>

#include "json.hpp"

/*
Defines extensions for Nlohmann's Json library to perform additional conversions that are missing.
*/

// Convert optional
namespace std {
	template <typename T>
	void to_json(nlohmann::json& j, const std::optional<T>& a) {
		if (a.has_value()) {
			j = a.value();
		}
	}

	template <typename T>
	void from_json(const nlohmann::json& j, std::optional<T>& a) {
		if (j.is_null()) {
			a = std::optional<T>();
		}
		else {
			a = std::optional<T>(j.get<T>());
		}
	}
}