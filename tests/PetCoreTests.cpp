#include "yuumi/BehaviorEngine.hpp"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>

int main() {
    using namespace std::chrono_literals;

    yuumi::BehaviorEngine pet;
    const double initialHunger = pet.state().hunger;
    for (int second = 0; second < 120; ++second) {
        pet.update(1s);
    }
    if (pet.state().hunger <= initialHunger) {
        return 1;
    }

    pet.feed(yuumi::availableFoods().front());
    if (pet.state().hunger >= initialHunger) {
        return 2;
    }
    const double energyBeforePlay = pet.state().energy;
    if (!pet.play() || pet.state().energy >= energyBeforePlay) {
        return 3;
    }

    const char* saveFile = "yuumi-test-save.sav";
    if (!pet.save(saveFile)) {
        return 4;
    }
    yuumi::BehaviorEngine restored;
    if (!restored.load(saveFile) ||
        std::abs(restored.state().hunger - pet.state().hunger) >= 0.001) {
        std::remove(saveFile);
        return 5;
    }
    std::remove(saveFile);

    {
        std::ofstream invalidSave(saveFile);
        invalidSave << "YUUMI_PET 1\n101 50 50\n";
    }
    if (restored.load(saveFile)) {
        std::remove(saveFile);
        return 6;
    }
    std::remove(saveFile);
}