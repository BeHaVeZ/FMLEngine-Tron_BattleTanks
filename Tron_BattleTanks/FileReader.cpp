#include "FileReader.h"
#include "Logger.h"
#include <fstream>
#include <sstream>

namespace FML
{
	FileReader::FileReader(const std::string& filename) : filename(filename) {}

	std::vector<std::string> FileReader::ReadLines()
	{
		std::vector<std::string> lines;
		std::ifstream file(filename);
		if (!file.is_open())
		{
			Logger::Log(LogLevel::Error, "Failed to open data file: %s", filename.c_str());
			return lines;
		}
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
		auto lines = ReadLines();

		for (const auto& line : lines) 
		{
			SDL_Rect rect{};
			std::istringstream iss(line);
			char openingBrace = 0;
			char comma1 = 0;
			char comma2 = 0;
			char comma3 = 0;
			char closingBrace = 0;

			if (!(iss >> openingBrace >> rect.x >> comma1 >> rect.y >> comma2
				>> rect.w >> comma3 >> rect.h >> closingBrace)
				|| openingBrace != '{' || comma1 != ',' || comma2 != ','
				|| comma3 != ',' || closingBrace != '}')
			{
				Logger::Log(LogLevel::Error, "Skipping malformed rectangle in %s: %s", filename.c_str(), line.c_str());
				continue;
			}

			rects.push_back(rect);
		}

		return rects;
	}
}