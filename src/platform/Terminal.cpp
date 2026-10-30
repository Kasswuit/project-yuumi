#include "yuumi/platform/Terminal.hpp"

#include <cstdio>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

namespace yuumi::platform {

namespace {
constexpr const char* enterScreen = "\x1b[?1049h\x1b[?25l";
constexpr const char* leaveScreen = "\x1b[?25h\x1b[?1049l";
}  // namespace

struct Terminal::Impl {
#ifdef _WIN32
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD savedMode = 0;
	bool modeSaved = false;
#else
	termios saved{};
	bool modeSaved = false;
#endif
};

Terminal::Terminal() : impl_(std::make_unique<Impl>()) {
#ifdef _WIN32
	if (GetConsoleMode(impl_->output, &impl_->savedMode)) {
		impl_->modeSaved = true;
		SetConsoleMode(impl_->output, impl_->savedMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	}
#else
	if (tcgetattr(STDIN_FILENO, &impl_->saved) == 0) {
		impl_->modeSaved = true;
		termios raw = impl_->saved;
		raw.c_lflag &= ~static_cast<tcflag_t>(ICANON | ECHO | ISIG);
		raw.c_cc[VMIN] = 0;
		raw.c_cc[VTIME] = 0;
		tcsetattr(STDIN_FILENO, TCSANOW, &raw);
	}
#endif
	write(enterScreen);
}

Terminal::~Terminal() {
	write(leaveScreen);
#ifdef _WIN32
	if (impl_->modeSaved) {
		SetConsoleMode(impl_->output, impl_->savedMode);
	}
#else
	if (impl_->modeSaved) {
		tcsetattr(STDIN_FILENO, TCSANOW, &impl_->saved);
	}
#endif
}

void Terminal::write(const std::string& text) {
	std::fwrite(text.data(), 1, text.size(), stdout);
	std::fflush(stdout);
}

int Terminal::readKey() {
#ifdef _WIN32
	if (!_kbhit()) {
		return -1;
	}
	const int key = _getch();
	if (key == 0 || key == 0xE0) {
		_getch();  // discard the second code of arrow/function keys
		return -1;
	}
	return key;
#else
	unsigned char key = 0;
	return read(STDIN_FILENO, &key, 1) == 1 ? key : -1;
#endif
}

}  // namespace yuumi::platform
