#pragma once

#include "yuumi/PetState.hpp"

#include <chrono>
#include <string>

namespace yuumi {

class BehaviorEngine {
public:
	const PetState& state() const;

	void update(std::chrono::duration<double> elapsed);
	void feed();
	bool play();
	void rest();

	bool save(const std::string& filePath) const;
	bool load(const std::string& filePath);

private:
	PetState pet_;
};

}  // namespace yuumi
