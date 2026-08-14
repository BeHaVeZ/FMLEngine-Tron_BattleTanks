#include "DebugDraw.h"
#include <SDL.h>
#include <algorithm>
#include <cmath>

namespace FML
{

	std::vector<DebugDraw::Line> DebugDraw::lines;
	std::vector<DebugDraw::Circle> DebugDraw::circles;
	std::vector<DebugDraw::Rectangle> DebugDraw::rectangles;
	std::vector<DebugDraw::Point> DebugDraw::points;

	void DebugDraw::ApplyColor(SDL_Renderer* renderer, const glm::vec4& color)
	{
		const auto channel = [](float value)
			{
				return static_cast<Uint8>(std::clamp(value, 0.f, 1.f) * 255.f);
			};

		SDL_SetRenderDrawColor(renderer, channel(color.r), channel(color.g), channel(color.b), channel(color.a));
	}

	void DebugDraw::DrawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color)
	{
		lines.push_back({ start, end, color });
	}

	void DebugDraw::DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color)
	{
		circles.push_back({ center, radius, color });
	}

	void DebugDraw::DrawRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		rectangles.push_back({ position, size, color, false });
	}

	void DebugDraw::DrawFilledRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		rectangles.push_back({ position, size, color, true });
	}

	void DebugDraw::DrawPoint(const glm::vec2& position, const glm::vec4& color)
	{
		points.push_back({ position, color });
	}

	void DebugDraw::DrawForwardVector(SDL_Renderer* renderer, const glm::vec2& position, float rotationDegrees, float length)
	{
		const float angleRadians = glm::radians(rotationDegrees - 90.f);
		const glm::vec2 end = position + glm::vec2(std::cos(angleRadians), std::sin(angleRadians)) * length;

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_RenderDrawLine(renderer, static_cast<int>(position.x), static_cast<int>(position.y),
			static_cast<int>(end.x), static_cast<int>(end.y));
	}

	void DebugDraw::Render(SDL_Renderer* renderer)
	{
		SDL_BlendMode previousBlendMode{};
		SDL_GetRenderDrawBlendMode(renderer, &previousBlendMode);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

		for (const auto& rect : rectangles)
		{
			if (!rect.filled)
				continue;

			ApplyColor(renderer, rect.color);
			const SDL_Rect sdlRect{ static_cast<int>(rect.position.x), static_cast<int>(rect.position.y),
				static_cast<int>(rect.size.x), static_cast<int>(rect.size.y) };
			SDL_RenderFillRect(renderer, &sdlRect);
		}

		for (const auto& rect : rectangles)
		{
			if (rect.filled)
				continue;

			ApplyColor(renderer, rect.color);
			const SDL_Rect sdlRect{ static_cast<int>(rect.position.x), static_cast<int>(rect.position.y),
				static_cast<int>(rect.size.x), static_cast<int>(rect.size.y) };
			SDL_RenderDrawRect(renderer, &sdlRect);
		}
		rectangles.clear();

		for (const auto& line : lines)
		{
			ApplyColor(renderer, line.color);
			SDL_RenderDrawLine(renderer, static_cast<int>(line.start.x), static_cast<int>(line.start.y),
				static_cast<int>(line.end.x), static_cast<int>(line.end.y));
		}
		lines.clear();

		constexpr int circleSegments = 24;
		for (const auto& circle : circles)
		{
			ApplyColor(renderer, circle.color);
			for (int i = 0; i < circleSegments; ++i)
			{
				const float angle1 = 2.f * static_cast<float>(M_PI) * i / circleSegments;
				const float angle2 = 2.f * static_cast<float>(M_PI) * (i + 1) / circleSegments;

				SDL_RenderDrawLine(renderer,
					static_cast<int>(circle.center.x + circle.radius * std::cos(angle1)),
					static_cast<int>(circle.center.y + circle.radius * std::sin(angle1)),
					static_cast<int>(circle.center.x + circle.radius * std::cos(angle2)),
					static_cast<int>(circle.center.y + circle.radius * std::sin(angle2)));
			}
		}
		circles.clear();

		for (const auto& point : points)
		{
			ApplyColor(renderer, point.color);
			SDL_RenderDrawPoint(renderer, static_cast<int>(point.position.x), static_cast<int>(point.position.y));
		}
		points.clear();

		SDL_SetRenderDrawBlendMode(renderer, previousBlendMode);
	}
}