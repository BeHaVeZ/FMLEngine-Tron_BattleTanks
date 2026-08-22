#define WIN32_LEAN_AND_MEAN

#if defined(_DEBUG) && defined(USE_VLD)
#include <vld.h>
#endif

#include "Game.h"
#include "GameScenes.h"
#include "DebugControls.h"

int main(int, char* [])
{
	FML::Game game;
	if (!game.Initialize("Tron Battle Tanks - Alexander Terentyev"))
	{
		return -1;
	}

	FML::GameScenes::Register();
	FML::DebugControls::PrintControls();

	game.Run();
	game.Cleanup();

	return 0;
}
