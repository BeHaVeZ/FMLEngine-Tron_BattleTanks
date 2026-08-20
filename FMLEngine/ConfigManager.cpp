#include "ConfigManager.h"
#include "ServiceLocator.h"
#include <SDL.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

namespace
{
	std::string Trim(const std::string& value)
	{
		static const std::string whitespace = " \t\r\n";
		static const std::string utf8Bom = "\xEF\xBB\xBF";

		size_t first = value.find_first_not_of(whitespace);
		if (first == std::string::npos) return {};

		if (value.compare(first, utf8Bom.size(), utf8Bom) == 0) first += utf8Bom.size();

		const size_t last = value.find_last_not_of(whitespace);
		if (first > last) return {};

		return value.substr(first, last - first + 1);
	}
}

namespace FML
{
	const std::vector<std::pair<int, int>>& ConfigManager::GetResolutionOptions()
	{
		static const std::vector<std::pair<int, int>> options{
			{ 1024, 768 },
			{ 1280, 960 },
			{ 1280, 720 },
			{ 1600, 900 },
			{ 1920, 1080 }
		};
		return options;
	}

	const std::vector<int>& ConfigManager::GetFpsLimitOptions()
	{
		static const std::vector<int> options{ 0, 30, 60, 120, 144 };
		return options;
	}

	void ConfigManager::SetDisplayResolution(int width, int height)
	{
		if (width <= 0 || height <= 0) return;

		displayWidth = width;
		displayHeight = height;
	}

	void ConfigManager::SetMasterVolume(float volume)
	{
		masterVolume = std::clamp(volume, 0.f, 1.f);
	}

	void ConfigManager::AttachWindow(SDL_Window* attachedWindow, SDL_Renderer* attachedRenderer)
	{
		window = attachedWindow;
		renderer = attachedRenderer;
	}

	void ConfigManager::ApplyDisplaySettings()
	{
		if (!window) return;

		SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

		if (!fullscreen)
		{
			SDL_SetWindowSize(window, displayWidth, displayHeight);
			SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		}

		ApplyScaling();
	}

	void ConfigManager::ApplyScaling()
	{
		if (!renderer) return;

		SDL_RenderSetLogicalSize(renderer, 0, 0);

		if (scalingMode == ScalingMode::Fit)
		{
			SDL_RenderSetLogicalSize(renderer, logicalWidth, logicalHeight);
			return;
		}

		int outputWidth = 0;
		int outputHeight = 0;
		if (SDL_GetRendererOutputSize(renderer, &outputWidth, &outputHeight) != 0) return;
		if (outputWidth <= 0 || outputHeight <= 0) return;

		SDL_RenderSetScale(renderer,
			static_cast<float>(outputWidth) / static_cast<float>(logicalWidth),
			static_cast<float>(outputHeight) / static_cast<float>(logicalHeight));
	}

	void ConfigManager::WindowToLogical(int windowX, int windowY, float& logicalX, float& logicalY) const
	{
		logicalX = static_cast<float>(windowX);
		logicalY = static_cast<float>(windowY);

		if (scalingMode == ScalingMode::Fit || !renderer) return;

		int outputWidth = 0;
		int outputHeight = 0;
		if (SDL_GetRendererOutputSize(renderer, &outputWidth, &outputHeight) != 0) return;
		if (outputWidth <= 0 || outputHeight <= 0) return;

		logicalX = logicalX * static_cast<float>(logicalWidth) / static_cast<float>(outputWidth);
		logicalY = logicalY * static_cast<float>(logicalHeight) / static_cast<float>(outputHeight);
	}

	void ConfigManager::ApplyVolume() const
	{
		ServiceLocator::GetSoundSystem().SetVolume(masterVolume);
	}

	void ConfigManager::Load()
	{
		std::ifstream file(settingsPath);
		if (!file.is_open()) return;

		std::string line;
		while (std::getline(file, line))
		{
			const size_t separator = line.find('=');
			if (separator == std::string::npos) continue;

			const std::string key = Trim(line.substr(0, separator));
			const std::string value = Trim(line.substr(separator + 1));
			if (value.empty()) continue;

			std::istringstream stream(value);

			if (key == "displayWidth")
			{
				int parsed = 0;
				if (stream >> parsed && parsed > 0) displayWidth = parsed;
			}
			else if (key == "displayHeight")
			{
				int parsed = 0;
				if (stream >> parsed && parsed > 0) displayHeight = parsed;
			}
			else if (key == "fullscreen")
			{
				int parsed = 0;
				if (stream >> parsed) fullscreen = parsed != 0;
			}
			else if (key == "fpsLimit")
			{
				int parsed = 0;
				if (stream >> parsed && parsed >= 0) fpsLimit = parsed;
			}
			else if (key == "masterVolume")
			{
				float parsed = 0.f;
				if (stream >> parsed) SetMasterVolume(parsed);
			}
			else if (key == "scaling")
			{
				if (value == "fit") scalingMode = ScalingMode::Fit;
				else if (value == "stretch") scalingMode = ScalingMode::Stretch;
			}
		}
	}

	void ConfigManager::Save() const
	{
		std::ofstream file(settingsPath, std::ios::trunc);
		if (!file.is_open()) return;

		file << "displayWidth=" << displayWidth << '\n';
		file << "displayHeight=" << displayHeight << '\n';
		file << "fullscreen=" << (fullscreen ? 1 : 0) << '\n';
		file << "fpsLimit=" << fpsLimit << '\n';
		file << "masterVolume=" << masterVolume << '\n';
		file << "scaling=" << (scalingMode == ScalingMode::Fit ? "fit" : "stretch") << '\n';
	}
}
