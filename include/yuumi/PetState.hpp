#pragma once

#include <chrono>

namespace yuumi {

struct PetState {
	double hunger = 25.0;
	double happiness = 75.0;
	double energy = 80.0;

	void update(std::chrono::duration<double> elapsed);
	void feed();
	bool play();
	void rest();
	bool isValid() const;
};

}  // namespace yuumi
