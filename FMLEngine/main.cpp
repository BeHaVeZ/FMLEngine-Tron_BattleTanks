#define WIN32_LEAN_AND_MEAN
#include "vld.h"
#include "Game.h"


//#define ANSI_COLOR_RESET "\033[0m"
//#define ANSI_COLOR_GREEN "\033[32m"
//#define ANSI_COLOR_WHITE "\033[37m"
//#define ANSI_COLOR_RED "\033[31m"
//#define ANSI_COLOR_ORANGE "\033[33m"
//
//
//#define LOG_TRACE(message) std::cout << ANSI_COLOR_GREEN << "[TRACE] " << message << ANSI_COLOR_RESET << std::endl
//#define LOG_INFO(message) std::cout << ANSI_COLOR_WHITE << "[INFO] " << message << ANSI_COLOR_RESET << std::endl
//#define LOG_CRITICAL(message) std::cerr << ANSI_COLOR_RED << "[CRITICAL] " << message << ANSI_COLOR_RESET << std::endl
//#define LOG_WARNING(message) std::cout << ANSI_COLOR_ORANGE << "[WARNING] " << message << ANSI_COLOR_RESET << std::endl

int main(int, char*[]) {
	Game game;
	if (!game.Initialize()) {
		return -1;
	}

	game.Run();

	game.Cleanup();

	return 0;
}