#include "yuumi/BehaviorEngine.hpp"

#include <array>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace {

constexpr std::string_view defaultSaveFile = "yuumi.sav";

void showPet(const yuumi::PetState& pet) {
	const std::array<std::string_view, 3> sprite = pet.energy < 20
		? std::array<std::string_view, 3>{"  /\\_/\\", " ( -.- )", "  z ^ z"}
		: pet.hunger >= 70
			? std::array<std::string_view, 3>{"  /\\_/\\", " ( o.o )", "  > ~ <"}
			: std::array<std::string_view, 3>{"  /\\_/\\", " ( ^.^ )", "  > ^ <"};

	for (const auto line : sprite) {
		std::cout << line << '\n';
	}
	std::cout << "Hunger: " << std::lround(pet.hunger) << "/100"
			  << "  Happiness: " << std::lround(pet.happiness) << "/100"
			  << "  Energy: " << std::lround(pet.energy) << "/100\n";
}

std::string argumentAfter(const std::string& line) {
	std::istringstream input(line);
	std::string command;
	std::string argument;
	input >> command;
	std::getline(input, argument);

	const auto first = argument.find_first_not_of(" \t");
	return first == std::string::npos ? std::string{} : argument.substr(first);
}

void showHelp() {
	std::cout << "Commands: feed, play, rest, save [file], load [file], help, quit\n"
				 "Needs change with real time while the game is running.\n";
}

}  // namespace

int main() {
	yuumi::BehaviorEngine pet;
	auto previousUpdate = std::chrono::steady_clock::now();
	std::string line;

	std::cout << "Yuumi Pet\n";
	showHelp();

	while (true) {
		showPet(pet.state());
		std::cout << "> ";
		if (!std::getline(std::cin, line)) {
			break;
		}

		const auto now = std::chrono::steady_clock::now();
		pet.update(now - previousUpdate);
		previousUpdate = now;

		std::istringstream commandInput(line);
		std::string command;
		commandInput >> command;

		if (command == "feed") {
			pet.feed();
			std::cout << "Yuumi enjoyed the snack.\n";
		} else if (command == "play") {
			std::cout << (pet.play() ? "Playtime!\n" : "Yuumi is too tired to play. Try rest.\n");
		} else if (command == "rest") {
			pet.rest();
			std::cout << "Yuumi had a little rest.\n";
		} else if (command == "save" || command == "load") {
			const std::string filePath = argumentAfter(line).empty()
				? std::string(defaultSaveFile)
				: argumentAfter(line);
			const bool succeeded = command == "save" ? pet.save(filePath) : pet.load(filePath);
			std::cout << (succeeded ? "Done.\n" : "Could not " + command + " that file.\n");
		} else if (command == "help") {
			showHelp();
		} else if (command == "quit" || command == "exit") {
			break;
		} else if (!command.empty()) {
			std::cout << "Unknown command. Type help to see the commands.\n";
		}
	}

	return 0;
}
