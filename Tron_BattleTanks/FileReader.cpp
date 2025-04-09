#include "FileReader.h"
#include <fstream>
#include <sstream>

namespace FML
{
	FileReader::FileReader(const std::string& filename) : filename(filename) {}

	std::vector<std::string> FileReader::readLines() 
	{
		std::vector<std::string> lines;
		std::ifstream file(filename);
		std::string line;

		while (std::getline(file, line)) 
		{
			if (!line.empty() && line[0] != '/') 
			{
				lines.push_back(line);
			}
		}

		return lines;
	}

	std::vector<SDL_Rect> FileReader::ReadRectangles() 
	{
		std::vector<SDL_Rect> rects;
		auto lines = readLines();

		for (const auto& line : lines) 
		{
			SDL_Rect rect;
			std::istringstream iss(line);
			char ignore;

			iss >> ignore >> rect.x >> ignore >> rect.y >> ignore
				>> rect.w >> ignore >> rect.h >> ignore;

			rects.push_back(rect);
		}

		return rects;
	}
}