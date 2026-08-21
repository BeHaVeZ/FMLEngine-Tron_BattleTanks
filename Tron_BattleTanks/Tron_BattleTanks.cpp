#define WIN32_LEAN_AND_MEAN

#if defined(_DEBUG) && defined(USE_VLD)
#include <vld.h>
#endif

#include "Game.h"
#include "DebugControls.h"

int main(int, char* [])
{
	FML::Game game;
	if (!game.Initialize()) 
	{
		return -1;
	}

	FML::DebugControls::PrintControls();

	game.Run();

	game.Cleanup();

	return 0;
}