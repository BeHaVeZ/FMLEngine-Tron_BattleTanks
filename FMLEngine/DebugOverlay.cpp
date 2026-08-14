#include "DebugOverlay.h"
#include "DebugDraw.h"
#include "Logger.h"
#include <algorithm>
#include <array>
#include <utility>

namespace FML
{
	namespace
	{
		struct ChannelInfo
		{
			DebugChannel channel;
			const char* key;
			const char* name;
		};

		constexpr std::array<ChannelInfo, 8> channels{ {
			{ DebugChannel::NavGrid,    "1", "Nav grid" },
			{ DebugChannel::Clearance,  "2", "Clearance" },
			{ DebugChannel::Paths,      "3", "Paths" },
			{ DebugChannel::Whiskers,   "4", "Whiskers" },
			{ DebugChannel::Perception, "5", "Perception" },
			{ DebugChannel::AgentState, "6", "Agent state" },
			{ DebugChannel::Colliders,  "7", "Colliders" },
			{ DebugChannel::Stats,      "8", "Stats" },
		} };

		constexpr glm::vec4 panelBackground{ 0.f, 0.f, 0.f, .72f };
		constexpr glm::vec4 enabledColor{ .35f, 1.f, .55f, 1.f };
		constexpr glm::vec4 disabledColor{ .45f, .45f, .5f, 1.f };
		constexpr glm::vec4 headingColor{ 1.f, .85f, .2f, 1.f };
		constexpr glm::vec4 focusColor{ 1.f, .45f, .85f, 1.f };
	}

	DebugOverlay& DebugOverlay::Instance()
	{
		static DebugOverlay instance;
		return instance;
	}

	void DebugOverlay::Initialize(SDL_Renderer*, const std::string& fontPath, int fontSize)
	{
		if (font)
			return;

		font = TTF_OpenFont(fontPath.c_str(), fontSize);
		if (!font)
		{
			Logger::Log(LogLevel::Error, "DebugOverlay could not open font '%s': %s", fontPath.c_str(), TTF_GetError());
		}
	}

	void DebugOverlay::Shutdown()
	{
		for (auto& [text, cached] : textCache)
		{
			if (cached.texture)
				SDL_DestroyTexture(cached.texture);
		}
		textCache.clear();

		if (font)
		{
			TTF_CloseFont(font);
			font = nullptr;
		}
	}

	void DebugOverlay::ToggleChannel(DebugChannel channel)
	{
		channelMask ^= static_cast<uint32_t>(channel);
	}

	bool DebugOverlay::IsEnabled(DebugChannel channel) const
	{
		return masterEnabled && (channelMask & static_cast<uint32_t>(channel)) != 0;
	}

	void DebugOverlay::WorldText(const glm::vec2& position, std::string text, const glm::vec4& color)
	{
		if (!masterEnabled)
			return;

		labels.push_back({ position, std::move(text), color });
	}

	void DebugOverlay::Stat(std::string line)
	{
		if (!IsEnabled(DebugChannel::Stats))
			return;

		stats.push_back(std::move(line));
	}

	void DebugOverlay::SubmitFocusCandidate(const void* agent, const glm::vec2& position, std::string name)
	{
		if (!masterEnabled || !agent)
			return;

		candidates.push_back({ agent, position, std::move(name) });
	}

	bool DebugOverlay::IsFocused(const void* agent) const
	{
		return focusedAgent == nullptr || focusedAgent == agent;
	}

	void DebugOverlay::CycleFocus()
	{
		if (previousCandidates.empty())
		{
			focusedAgent = nullptr;
			return;
		}

		const auto current = std::find_if(previousCandidates.begin(), previousCandidates.end(),
			[this](const FocusCandidate& candidate) { return candidate.agent == focusedAgent; });

		if (current == previousCandidates.end())
		{
			focusedAgent = previousCandidates.front().agent;
			return;
		}

		const auto next = std::next(current);
		focusedAgent = (next == previousCandidates.end()) ? nullptr : next->agent;
	}

	void DebugOverlay::FocusStat(const void* agent, std::string line)
	{
		if (!masterEnabled || focusedAgent == nullptr || focusedAgent != agent)
			return;

		focusStats.push_back(std::move(line));
	}

	const DebugOverlay::CachedText* DebugOverlay::AcquireText(SDL_Renderer* renderer, const std::string& text)
	{
		if (!font || text.empty())
			return nullptr;

		if (auto it = textCache.find(text); it != textCache.end())
		{
			it->second.lastUsedFrame = frameCounter;
			return &it->second;
		}

		SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), SDL_Color{ 255, 255, 255, 255 });
		if (!surface)
			return nullptr;

		CachedText cached;
		cached.texture = SDL_CreateTextureFromSurface(renderer, surface);
		cached.width = surface->w;
		cached.height = surface->h;
		cached.lastUsedFrame = frameCounter;
		SDL_FreeSurface(surface);

		if (!cached.texture)
			return nullptr;

		return &(textCache[text] = cached);
	}

	void DebugOverlay::EvictStaleText()
	{
		for (auto it = textCache.begin(); it != textCache.end();)
		{
			if (frameCounter - it->second.lastUsedFrame > textEvictionFrames)
			{
				if (it->second.texture)
					SDL_DestroyTexture(it->second.texture);
				it = textCache.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void DebugOverlay::DrawText(SDL_Renderer* renderer, const std::string& text, glm::vec2 position, const glm::vec4& color)
	{
		const CachedText* cached = AcquireText(renderer, text);
		if (!cached)
			return;

		const auto channel = [](float value) { return static_cast<Uint8>(std::clamp(value, 0.f, 1.f) * 255.f); };
		SDL_SetTextureColorMod(cached->texture, channel(color.r), channel(color.g), channel(color.b));
		SDL_SetTextureAlphaMod(cached->texture, channel(color.a));

		const SDL_Rect destination{ static_cast<int>(position.x), static_cast<int>(position.y), cached->width, cached->height };
		SDL_RenderCopy(renderer, cached->texture, nullptr, &destination);
	}

	std::vector<DebugOverlay::PanelLine> DebugOverlay::BuildPanelLines(SDL_Renderer* renderer)
	{
		std::vector<PanelLine> lines;
		lines.emplace_back("F3 DEBUG", headingColor);

		for (const ChannelInfo& info : channels)
		{
			const bool on = (channelMask & static_cast<uint32_t>(info.channel)) != 0;
			lines.emplace_back(std::string(info.key) + "  " + info.name + (on ? "  [on]" : ""), on ? enabledColor : disabledColor);
		}

		lines.emplace_back("F4  focus: " + DescribeFocus(), focusedAgent ? focusColor : disabledColor);

		if (!focusStats.empty())
		{
			for (const std::string& detail : focusStats)
			{
				lines.emplace_back("    " + detail, focusColor);
			}
		}

		if (IsEnabled(DebugChannel::Stats) && !stats.empty())
		{
			lines.emplace_back("", headingColor);
			for (const std::string& stat : stats)
			{
				lines.emplace_back(stat, glm::vec4{ .8f, .9f, 1.f, 1.f });
			}
		}

		int widest = 0;
		for (const auto& [text, color] : lines)
		{
			if (const CachedText* cached = AcquireText(renderer, text))
				widest = std::max(widest, cached->width);
		}

		DebugDraw::DrawFilledRectangle(
			panelOrigin - glm::vec2{ static_cast<float>(panelPadding) },
			{ static_cast<float>(widest + panelPadding * 2), static_cast<float>(lines.size() * lineHeight + panelPadding * 2) },
			panelBackground);

		return lines;
	}

	void DebugOverlay::DrawPanelText(SDL_Renderer* renderer, const std::vector<PanelLine>& lines)
	{
		for (size_t i = 0; i < lines.size(); ++i)
		{
			DrawText(renderer, lines[i].first, panelOrigin + glm::vec2{ 0.f, static_cast<float>(i * lineHeight) }, lines[i].second);
		}
	}

	std::string DebugOverlay::DescribeFocus() const
	{
		if (!focusedAgent)
			return "all (" + std::to_string(candidates.size()) + ")";

		for (size_t i = 0; i < candidates.size(); ++i)
		{
			if (candidates[i].agent == focusedAgent)
				return candidates[i].name + " " + std::to_string(i + 1) + "/" + std::to_string(candidates.size());
		}

		return "(gone)";
	}

	void DebugOverlay::HighlightFocusedAgent() const
	{
		if (!focusedAgent)
			return;

		for (const FocusCandidate& candidate : candidates)
		{
			if (candidate.agent != focusedAgent)
				continue;

			DebugDraw::DrawCircle(candidate.position, 22.f, focusColor);
			DebugDraw::DrawCircle(candidate.position, 24.f, focusColor);
			return;
		}
	}

	void DebugOverlay::Render(SDL_Renderer* renderer)
	{
		++frameCounter;

		if (masterEnabled)
		{
			if (focusedAgent && std::none_of(candidates.begin(), candidates.end(),
				[this](const FocusCandidate& candidate) { return candidate.agent == focusedAgent; }))
			{
				focusedAgent = nullptr;
			}

			HighlightFocusedAgent();
			const std::vector<PanelLine> panelLines = BuildPanelLines(renderer);
			DebugDraw::Render(renderer);

			for (const Label& label : labels)
			{
				DrawText(renderer, label.text, label.position, label.color);
			}

			DrawPanelText(renderer, panelLines);
		}

		labels.clear();
		stats.clear();
		focusStats.clear();
		previousCandidates = std::move(candidates);
		candidates.clear();
		EvictStaleText();
	}
}
