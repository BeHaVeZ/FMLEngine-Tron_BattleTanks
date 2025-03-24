#define WIN32_LEAN_AND_MEAN
#include "vld.h"
#include "Game.h"


	int main(int, char* []) {
		FML::Game game;
		if (!game.Initialize()) {
			return -1;
		}

		game.Run();

		game.Cleanup();

		return 0;
	}
