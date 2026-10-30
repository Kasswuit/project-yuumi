#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace yuumi::ui {

// Fixed-size character grid; sized so it can later map onto an LCD.
class Screen {
public:
	Screen(int rows, int cols);

	int rows() const { return rows_; }
	int cols() const { return cols_; }

	void clear();
	void put(int row, int col, std::string_view text);

	// Returns the terminal output that turns the last presented frame into the current one.
	std::string diff();

private:
	int rows_;
	int cols_;
	bool cleared_ = false;
	std::vector<std::string> frame_;
	std::vector<std::string> shown_;
};

}  // namespace yuumi::ui
