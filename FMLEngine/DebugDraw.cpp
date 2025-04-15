#include "DebugDraw.h"
#include <SDL.h>
#include "Logger.h"

namespace FML
{

	std::vector<DebugDraw::Line> DebugDraw::lines;
	std::vector<DebugDraw::Circle> DebugDraw::circles;
	std::vector<DebugDraw::Rectangle> DebugDraw::rectangles;
	std::vector<DebugDraw::Point> DebugDraw::points;

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
		rectangles.push_back({ position, size, color });
	}

	void DebugDraw::DrawPoint(const glm::vec2& position, const glm::vec4& color) {
		points.push_back({ position, color });
	}

	void DebugDraw::DrawForwardVector(SDL_Renderer* renderer, const glm::vec2& position, float rotationDegrees, float length)
	{
		float angleRad = glm::radians(rotationDegrees - 90);
		glm::vec2 end = position + glm::vec2(cos(angleRad), sin(angleRad)) * length;

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_RenderDrawLine(renderer, static_cast<int>(position.x), static_cast<int>(position.y),
			static_cast<int>(end.x), static_cast<int>(end.y));
	}

	void DebugDraw::Render(SDL_Renderer* renderer)
	{
		for (const auto& line : lines)
		{
			SDL_SetRenderDrawColor(renderer, (Uint8)line.color.r * 255, (Uint8)line.color.g * 255, (Uint8)line.color.b * 255, (Uint8)line.color.a * 255);
			SDL_RenderDrawLine(renderer, (int)line.start.x, (int)line.start.y, (int)line.end.x, (int)line.end.y);
		}
		lines.clear();

		for (const auto& circle : circles)
		{
			SDL_SetRenderDrawColor(renderer, (Uint8)circle.color.r * 255, (Uint8)circle.color.g * 255, (Uint8)circle.color.b * 255, (Uint8)circle.color.a * 255);
			int numSegments = 32;
			for (int i = 0; i < numSegments; ++i) {
				float angle1 = 2.0f * (float)M_PI * i / numSegments;
				float angle2 = 2.0f * (float)M_PI * (i + 1) / numSegments;


				SDL_RenderDrawLine(renderer,
					static_cast<int>(circle.center.x + circle.radius * cos(angle1)), static_cast<int>(circle.center.y + circle.radius * sin(angle1)),
					static_cast<int>(circle.center.x + circle.radius * cos(angle2)), static_cast<int>(circle.center.y + circle.radius * sin(angle2)));
			}
		}
		circles.clear();

		for (const auto& rect : rectangles)
		{
			SDL_SetRenderDrawColor(renderer, (Uint8)rect.color.r * 255, (Uint8)rect.color.g * 255, (Uint8)rect.color.b * 255, (Uint8)rect.color.a * 255);
			SDL_Rect sdlRect = { static_cast<int>(rect.position.x), static_cast<int>(rect.position.y), static_cast<int>(rect.size.x), static_cast<int>(rect.size.y) };
			SDL_RenderDrawRect(renderer, &sdlRect);
		}
		rectangles.clear();

		for (const auto& point : points)
		{
			SDL_SetRenderDrawColor(renderer, (Uint8)point.color.r * 255, (Uint8)point.color.g * 255, (Uint8)point.color.b * 255, (Uint8)point.color.a * 255);
			SDL_RenderDrawPoint(renderer, (int)point.position.x, (int)point.position.y);
		}
		points.clear();

	}
}