#include "yuumi/BehaviorEngine.hpp"

#include <array>
#include <chrono>
#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace {

constexpr std::string_view defaultSaveFile = "yuumi.sav";

using Art = std::array<std::string_view, 3>;

constexpr Art feedArt{"  /\\_/\\", " ( ^o^ ) nom", "  > U <"};
constexpr Art playArt{"  /\\_/\\", " ( >w< ) o", "  /| |\\"};
constexpr Art restArt{"  /\\_/\\", " ( -.- ) zZ", "  (___)"};
constexpr Art tiredArt{"  /\\_/\\", " ( x.x )", "  /   \\"};
constexpr Art saveArt{"  /\\_/\\", " ( ^_~ ) [saved]", "  > v <"};
constexpr Art loadArt{"  /\\_/\\", " ( o_o ) [loaded]", "  > ^ <"};

void showArt(const Art& art) {
	for (const auto line : art) {
		std::cout << line << '\n';
	}
}

void showPet(const yuumi::PetState& pet) {
	const Art sprite = pet.energy < 20
		? Art{"  /\\_/\\", " ( -.- )", "  z ^ z"}
		: pet.hunger >= 70
			? Art{"  /\\_/\\", " ( o.o )", "  > ~ <"}
			: Art{"  /\\_/\\", " ( ^.^ )", "  > ^ <"};

	showArt(sprite);
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

std::string lowered(std::string text) {
	for (char& c : text) {
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}
	return text;
}

// Accepts a menu number or a food name; returns nullptr if nothing matches.
const yuumi::Food* chooseFood(const std::string& choice) {
	const auto& foods = yuumi::availableFoods();
	const std::string wanted = lowered(choice);

	for (std::size_t i = 0; i < foods.size(); ++i) {
		if (wanted == std::to_string(i + 1) || wanted == lowered(foods[i].name)) {
			return &foods[i];
		}
	}
	return nullptr;
}

void feedPet(yuumi::BehaviorEngine& pet, std::string choice) {
	if (choice.empty()) {
		std::cout << "What should Yuumi eat?\n";
		const auto& foods = yuumi::availableFoods();
		for (std::size_t i = 0; i < foods.size(); ++i) {
			std::cout << "  " << i + 1 << ") " << foods[i].name << '\n';
		}
		std::cout << "Choice (number or name, blank to cancel): ";
		std::getline(std::cin, choice);
		if (choice.empty()) {
			std::cout << "Maybe later.\n";
			return;
		}
	}

	const yuumi::Food* food = chooseFood(choice);
	if (food == nullptr) {
		std::cout << "Yuumi doesn't know that food.\n";
		return;
	}

	pet.feed(*food);
	showArt(feedArt);
	std::cout << "Yuumi ate the " << food->name << ".\n";
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
			feedPet(pet, argumentAfter(line));
		} else if (command == "play") {
			if (pet.play()) {
				showArt(playArt);
				std::cout << "Playtime!\n";
			} else {
				showArt(tiredArt);
				std::cout << "Yuumi is too tired to play. Try rest.\n";
			}
		} else if (command == "rest") {
			pet.rest();
			showArt(restArt);
			std::cout << "Yuumi had a little rest.\n";
		} else if (command == "save" || command == "load") {
			const std::string filePath = argumentAfter(line).empty()
				? std::string(defaultSaveFile)
				: argumentAfter(line);
			const bool succeeded = command == "save" ? pet.save(filePath) : pet.load(filePath);
			if (succeeded) {
				showArt(command == "save" ? saveArt : loadArt);
			}
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
