#pragma once
#include <string>
#include <vector>
#include <SDL.h>

namespace FML
{
	class FileReader {
	public:
		FileReader(const std::string& filename);
		std::vector<SDL_Rect> ReadRectangles();

	private:
		std::string filename;
		std::vector<std::string> readLines();
	};

}

