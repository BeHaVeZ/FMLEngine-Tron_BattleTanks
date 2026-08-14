#pragma once
#include <string>
#include <vector>
#include <SDL.h>

namespace FML
{
	//REASON FOR NOT BEING IN ENGINE PROJECT:
	// This class is specific for the boxcolliders on the map for the tank game yk.
	class FileReader {
	public:
		FileReader(const std::string& filename);
		std::vector<SDL_Rect> ReadRectangles();
		std::vector<std::string> ReadLines();

	private:
		std::string filename;
	};

}

