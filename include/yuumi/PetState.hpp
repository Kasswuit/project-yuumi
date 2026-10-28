#pragma once

#include <chrono>
#include <vector>

namespace yuumi {

struct Food {
	const char* name;
	double hunger;
	double happiness;
	double energy;
};

const std::vector<Food>& availableFoods();

struct PetState {
	double hunger = 25.0;
	double happiness = 75.0;
	double energy = 80.0;

	void update(std::chrono::duration<double> elapsed);
	void feed(const Food& food);
	bool play();
	void rest();
	bool isValid() const;
};

}  // namespace yuumi
