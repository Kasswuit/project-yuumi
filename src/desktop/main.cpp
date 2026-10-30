#include "yuumi/BehaviorEngine.hpp"
#include "yuumi/platform/Terminal.hpp"
#include "yuumi/ui/Screen.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <cmath>
#include <string>
#include <string_view>
#include <thread>

namespace {

using namespace std::chrono_literals;
using Clock = std::chrono::steady_clock;

constexpr const char* saveFile = "yuumi.sav";
constexpr int screenRows = 12;
constexpr int screenCols = 42;
constexpr int ctrlC = 3;

using Art = std::array<std::string_view, 3>;
using Anim = std::array<Art, 2>;

constexpr Anim idleAnim{Art{" /\\_/\\", "( ^.^ )", " > ^ <"}, Art{" /\\_/\\", "( -.- )", " > ^ <"}};
constexpr Anim hungryAnim{Art{" /\\_/\\", "( o.o )", " > ~ <"}, Art{" /\\_/\\", "( o.O )", " > - <"}};
constexpr Anim sleepyAnim{Art{" /\\_/\\  z", "( -.- )", "  z^z"}, Art{" /\\_/\\  Z", "( -.- )", "  z^z"}};
constexpr Anim feedAnim{Art{" /\\_/\\", "( ^o^ ) nom", " > U <"}, Art{" /\\_/\\", "( ^-^ ) NOM", " > U <"}};
constexpr Anim playAnim{Art{" /\\_/\\", "( >w< )  o", " /| |\\"}, Art{" /\\_/\\", "( ^w^ ) o", " \\| |/"}};
constexpr Anim restAnim{Art{" /\\_/\\", "( -.- ) z", " (___)"}, Art{" /\\_/\\", "( -.- ) Z", " (___)"}};
constexpr Anim tiredAnim{Art{" /\\_/\\", "( x.x )", " /   \\"}, Art{" /\\_/\\", "( x.x )", " \\   /"}};
constexpr Anim saveAnim{Art{" /\\_/\\", "( ^_~ ) saved", " > v <"}, Art{" /\\_/\\", "( ^_^ ) saved", " > v <"}};
constexpr Anim loadAnim{Art{" /\\_/\\", "( o_o ) loaded", " > ^ <"}, Art{" /\\_/\\", "( O_O ) loaded", " > ^ <"}};

enum class Mode { Idle, FeedMenu };

struct Action {
	const Anim* anim = nullptr;
	Clock::time_point until{};
};

std::string bar(double value) {
	const int filled = std::clamp(static_cast<int>(std::lround(value / 10.0)), 0, 10);
	return "[" + std::string(filled, '#') + std::string(10 - filled, '.') + "]";
}

void putStat(yuumi::ui::Screen& screen, int row, std::string label, double value) {
	label.resize(10, ' ');
	screen.put(row, 1, label + bar(value) + ' ' + std::to_string(std::lround(value)));
}

void render(yuumi::ui::Screen& screen, const yuumi::PetState& pet, Mode mode, const Action& action,
			const std::string& message, Clock::time_point now, Clock::time_point start) {
	screen.clear();
	screen.put(0, 1, "YUUMI PET");
	screen.put(1, 0, std::string(screenCols, '-'));

	const bool acting = action.anim != nullptr && now < action.until;
	const Anim& anim = acting ? *action.anim
		: pet.energy < 20 ? sleepyAnim
		: pet.hunger >= 70 ? hungryAnim
		: idleAnim;
	const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
	const auto frame = static_cast<std::size_t>(elapsed / (acting ? 250 : 500)) % anim.size();
	for (std::size_t line = 0; line < anim[frame].size(); ++line) {
		screen.put(2 + static_cast<int>(line), 2, anim[frame][line]);
	}

	putStat(screen, 6, "Hunger", pet.hunger);
	putStat(screen, 7, "Happiness", pet.happiness);
	putStat(screen, 8, "Energy", pet.energy);
	screen.put(9, 0, std::string(screenCols, '-'));

	if (mode == Mode::FeedMenu) {
		std::string menu = "Feed:";
		const auto& foods = yuumi::availableFoods();
		for (std::size_t i = 0; i < foods.size(); ++i) {
			menu += ' ' + std::to_string(i + 1) + ' ' + foods[i].name;
		}
		screen.put(10, 1, menu);
		screen.put(11, 1, "Pick a number, any other key cancels");
	} else {
		screen.put(10, 1, message);
		screen.put(11, 1, "[f]eed [p]lay [r]est [s]ave [l]oad [q]uit");
	}
}

}  // namespace

int main() {
	yuumi::platform::Terminal terminal;
	yuumi::ui::Screen screen(screenRows, screenCols);
	yuumi::BehaviorEngine pet;

	const auto start = Clock::now();
	auto previous = start;
	Mode mode = Mode::Idle;
	Action action;
	std::string message = "Hello! Take care of Yuumi.";
	bool running = true;

	const auto perform = [&](const Anim& anim, std::string text) {
		action = {&anim, Clock::now() + 2s};
		message = std::move(text);
	};

	while (running) {
		const auto now = Clock::now();
		pet.update(now - previous);
		previous = now;

		for (int key = terminal.readKey(); key != -1; key = terminal.readKey()) {
			const int lowered = std::tolower(key);
			if (key == ctrlC) {
				running = false;
			} else if (mode == Mode::FeedMenu) {
				const auto& foods = yuumi::availableFoods();
				const int index = key - '1';
				mode = Mode::Idle;
				if (index >= 0 && index < static_cast<int>(foods.size())) {
					pet.feed(foods[index]);
					perform(feedAnim, std::string("Yuumi ate the ") + foods[index].name + ".");
				} else {
					message = "Maybe later.";
				}
			} else if (lowered == 'f') {
				mode = Mode::FeedMenu;
			} else if (lowered == 'p') {
				if (pet.play()) {
					perform(playAnim, "Playtime!");
				} else {
					perform(tiredAnim, "Too tired to play. Try rest.");
				}
			} else if (lowered == 'r') {
				pet.rest();
				perform(restAnim, "Yuumi had a little rest.");
			} else if (lowered == 's') {
				if (pet.save(saveFile)) {
					perform(saveAnim, "Saved.");
				} else {
					message = "Could not save.";
				}
			} else if (lowered == 'l') {
				if (pet.load(saveFile)) {
					perform(loadAnim, "Loaded.");
				} else {
					message = "Could not load.";
				}
			} else if (lowered == 'q') {
				running = false;
			}
		}

		render(screen, pet.state(), mode, action, message, now, start);
		const std::string output = screen.diff();
		if (!output.empty()) {
			terminal.write(output);
		}
		std::this_thread::sleep_for(50ms);
	}

	return 0;
}
