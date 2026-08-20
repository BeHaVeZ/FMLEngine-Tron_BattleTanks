#pragma once
#include <utility>
#include <vector>

struct SDL_Window;
struct SDL_Renderer;

namespace FML
{

	enum class ScalingMode
	{
		Fit,
		Stretch
	};

	class ConfigManager {
	public:
		static ConfigManager& Instance() {
			static ConfigManager instance;
			return instance;
		}

		int GetWindowWidth() const { return logicalWidth; }
		int GetWindowHeight() const { return logicalHeight; }

		int GetDisplayWidth() const { return displayWidth; }
		int GetDisplayHeight() const { return displayHeight; }
		void SetDisplayResolution(int width, int height);

		bool IsFullscreen() const { return fullscreen; }
		void SetFullscreen(bool enabled) { fullscreen = enabled; }

		int GetFpsLimit() const { return fpsLimit; }
		void SetFpsLimit(int fps) { fpsLimit = fps; }

		float GetMasterVolume() const { return masterVolume; }
		void SetMasterVolume(float volume);

		ScalingMode GetScalingMode() const { return scalingMode; }
		void SetScalingMode(ScalingMode mode) { scalingMode = mode; }

		void WindowToLogical(int windowX, int windowY, float& logicalX, float& logicalY) const;

		static const std::vector<std::pair<int, int>>& GetResolutionOptions();
		static const std::vector<int>& GetFpsLimitOptions();

		void Load();
		void Save() const;

		void AttachWindow(SDL_Window* attachedWindow, SDL_Renderer* attachedRenderer);
		void ApplyDisplaySettings();
		void ApplyScaling();
		void ApplyVolume() const;

		ConfigManager(const ConfigManager&) = delete;
		ConfigManager& operator=(const ConfigManager&) = delete;

	private:
		ConfigManager() = default;

		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;

		int logicalWidth = 1024;
		int logicalHeight = 768;

		int displayWidth = 1024;
		int displayHeight = 768;
		bool fullscreen = false;
		int fpsLimit = 60;
		float masterVolume = 0.5f;
		ScalingMode scalingMode = ScalingMode::Stretch;

		static constexpr const char* settingsPath = "settings.txt";
	};


}
