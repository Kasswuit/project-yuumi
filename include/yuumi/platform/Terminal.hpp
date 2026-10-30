#pragma once

#include <memory>
#include <string>

namespace yuumi::platform {

// Owns the console for the program's lifetime: alternate screen, hidden cursor, raw key input.
class Terminal {
public:
	Terminal();
	~Terminal();
	Terminal(const Terminal&) = delete;
	Terminal& operator=(const Terminal&) = delete;

	void write(const std::string& text);

	// Returns the next pressed key, or -1 if none is waiting.
	int readKey();

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};

}  // namespace yuumi::platform
