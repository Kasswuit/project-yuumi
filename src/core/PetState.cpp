#include "yuumi/PetState.hpp"

#include <algorithm>
#include <cmath>

namespace yuumi {
namespace {

double boundedStat(double value) {
	return std::clamp(value, 0.0, 100.0);
}

}  // namespace

void PetState::update(std::chrono::duration<double> elapsed) {
	const double minutes = std::max(0.0, elapsed.count()) / 60.0;
	hunger = boundedStat(hunger + minutes * 0.5);
	energy = boundedStat(energy - minutes * 0.25);

	const double happinessLoss = hunger >= 70 || energy <= 20 ? 0.3 : 0.05;
	happiness = boundedStat(happiness - minutes * happinessLoss);
}

void PetState::feed() {
	hunger = std::max(0.0, hunger - 30.0);
	happiness = std::min(100.0, happiness + 4.0);
}

bool PetState::play() {
	if (energy < 10) {
		return false;
	}

	happiness = std::min(100.0, happiness + 15.0);
	energy = std::max(0.0, energy - 20.0);
	hunger = std::min(100.0, hunger + 8.0);
	return true;
}

void PetState::rest() {
	energy = std::min(100.0, energy + 30.0);
	hunger = std::min(100.0, hunger + 5.0);
}

bool PetState::isValid() const {
	return std::isfinite(hunger) && hunger >= 0 && hunger <= 100 &&
		std::isfinite(happiness) && happiness >= 0 && happiness <= 100 &&
		std::isfinite(energy) && energy >= 0 && energy <= 100;
}

}  // namespace yuumi
