#include "yuumi/ui/Screen.hpp"

namespace yuumi::ui {

Screen::Screen(int rows, int cols)
	: rows_(rows),
	  cols_(cols),
	  frame_(rows, std::string(cols, ' ')),
	  shown_(rows, std::string(cols, '\0')) {}

void Screen::clear() {
	for (auto& line : frame_) {
		line.assign(static_cast<std::size_t>(cols_), ' ');
	}
}

void Screen::put(int row, int col, std::string_view text) {
	if (row < 0 || row >= rows_) {
		return;
	}
	for (std::size_t i = 0; i < text.size(); ++i) {
		const int target = col + static_cast<int>(i);
		if (target >= 0 && target < cols_) {
			frame_[row][target] = text[i];
		}
	}
}

std::string Screen::diff() {
	std::string out;
	if (!cleared_) {
		out += "\x1b[2J";
		cleared_ = true;
	}

	for (int row = 0; row < rows_; ++row) {
		int col = 0;
		while (col < cols_) {
			if (frame_[row][col] == shown_[row][col]) {
				++col;
				continue;
			}
			const int start = col;
			while (col < cols_ && frame_[row][col] != shown_[row][col]) {
				++col;
			}
			out += "\x1b[" + std::to_string(row + 1) + ';' + std::to_string(start + 1) + 'H';
			out.append(frame_[row], start, col - start);
		}
	}

	shown_ = frame_;
	return out;
}

}  // namespace yuumi::ui
