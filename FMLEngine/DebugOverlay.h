#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <glm.hpp>
#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace FML
{
	enum class DebugChannel : uint32_t
	{
		NavGrid = 1 << 0,
		Clearance = 1 << 1,
		Paths = 1 << 2,
		Avoidance = 1 << 3,
		Perception = 1 << 4,
		AgentState = 1 << 5,
		Colliders = 1 << 6,
		Stats = 1 << 7,
		Prediction = 1 << 8,
	};

	struct DebugChannelInfo
	{
		DebugChannel channel;
		SDL_Keycode key;
		const char* keyLabel;
		const char* name;
	};

	inline constexpr std::array<DebugChannelInfo, 9> DebugChannelInfos{ {
		{ DebugChannel::NavGrid,    SDLK_1, "1", "Nav grid"    },
		{ DebugChannel::Clearance,  SDLK_2, "2", "Clearance"   },
		{ DebugChannel::Paths,      SDLK_3, "3", "Paths"       },
		{ DebugChannel::Avoidance,  SDLK_4, "4", "Avoidance"   },
		{ DebugChannel::Perception, SDLK_5, "5", "Perception"  },
		{ DebugChannel::AgentState, SDLK_6, "6", "Agent state" },
		{ DebugChannel::Colliders,  SDLK_7, "7", "Colliders"   },
		{ DebugChannel::Stats,      SDLK_8, "8", "Stats"       },
		{ DebugChannel::Prediction, SDLK_9, "9", "Prediction"  },
	} };

	class DebugOverlay
	{
	public:
		static DebugOverlay& Instance();

		DebugOverlay(const DebugOverlay&) = delete;
		DebugOverlay& operator=(const DebugOverlay&) = delete;

		void Initialize(SDL_Renderer* renderer, const std::string& fontPath, int fontSize);
		void Shutdown();

		bool IsMasterEnabled() const { return masterEnabled; }
		void ToggleMaster() { masterEnabled = !masterEnabled; }
		void ToggleChannel(DebugChannel channel);
		bool IsEnabled(DebugChannel channel) const;
		bool IsChannelSelected(DebugChannel channel) const;

		void WorldText(const glm::vec2& position, std::string text, const glm::vec4& color = { 1.f, 1.f, 1.f, 1.f });
		void Stat(std::string line);

		void SubmitFocusCandidate(const void* agent, const glm::vec2& position, std::string_view name);
		bool IsFocused(const void* agent) const;
		bool HasFocus() const { return focusedAgent != nullptr; }
		void CycleFocus();
		void ResetFocus();
		void FocusStat(const void* agent, std::string line);

		void Render(SDL_Renderer* renderer);

	private:
		DebugOverlay() = default;

		struct Label
		{
			glm::vec2 position;
			std::string text;
			glm::vec4 color;
		};

		struct CachedText
		{
			SDL_Texture* texture{ nullptr };
			int width{ 0 };
			int height{ 0 };
			uint64_t lastUsedFrame{ 0 };
		};

		struct FocusCandidate
		{
			const void* agent{ nullptr };
			glm::vec2 position{};
			std::string name;
		};

		using PanelLine = std::pair<std::string, glm::vec4>;

		const CachedText* AcquireText(SDL_Renderer* renderer, const std::string& text);
		void EvictStaleText();
		void DrawText(SDL_Renderer* renderer, const std::string& text, glm::vec2 position, const glm::vec4& color);
		std::vector<PanelLine> BuildPanelLines(SDL_Renderer* renderer);
		void DrawPanelText(SDL_Renderer* renderer, const std::vector<PanelLine>& lines);
		std::string DescribeFocus() const;
		void HighlightFocusedAgent() const;

		TTF_Font* font{ nullptr };
		bool masterEnabled{ false };
		uint32_t channelMask{ static_cast<uint32_t>(DebugChannel::NavGrid) | static_cast<uint32_t>(DebugChannel::Paths) | static_cast<uint32_t>(DebugChannel::Stats) };

		std::vector<Label> labels;
		std::vector<std::string> stats;
		std::vector<std::string> focusStats;
		std::unordered_map<std::string, CachedText> textCache;
		uint64_t frameCounter{ 0 };

		const void* focusedAgent{ nullptr };
		std::vector<FocusCandidate> candidates;
		std::vector<FocusCandidate> previousCandidates;

		static constexpr uint64_t textEvictionFrames = 120;
		static constexpr int lineHeight = 14;
		static constexpr int panelPadding = 6;
		static constexpr glm::vec2 panelOrigin{ 8.f, 120.f };
	};

	inline bool DebugEnabled(DebugChannel channel)
	{
		return DebugOverlay::Instance().IsEnabled(channel);
	}
}
