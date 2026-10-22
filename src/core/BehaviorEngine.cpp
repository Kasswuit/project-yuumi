#include "yuumi/BehaviorEngine.hpp"

#include <fstream>
#include <iomanip>

namespace yuumi {

const PetState& BehaviorEngine::state() const {
	return pet_;
}

void BehaviorEngine::update(std::chrono::duration<double> elapsed) {
	pet_.update(elapsed);
}

void BehaviorEngine::feed() {
	pet_.feed();
}

bool BehaviorEngine::play() {
	return pet_.play();
}

void BehaviorEngine::rest() {
	pet_.rest();
}

bool BehaviorEngine::save(const std::string& filePath) const {
	std::ofstream output(filePath);
	if (!output) {
		return false;
	}

	output << std::fixed << std::setprecision(4)
		   << "YUUMI_PET 1\n"
		   << pet_.hunger << ' '
		   << pet_.happiness << ' '
		   << pet_.energy << '\n';
	return static_cast<bool>(output);
}

bool BehaviorEngine::load(const std::string& filePath) {
	std::ifstream input(filePath);
	std::string signature;
	int version = 0;
	PetState loaded;

	if (!(input >> signature >> version >> loaded.hunger >> loaded.happiness >> loaded.energy) ||
		signature != "YUUMI_PET" || version != 1 || !loaded.isValid()) {
		return false;
	}

	input >> std::ws;
	if (!input.eof()) {
		return false;
	}

	pet_ = loaded;
	return true;
}

}  // namespace yuumi
