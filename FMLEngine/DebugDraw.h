#pragma once

#include <vector>
#include <glm.hpp>
#include "SDL_render.h"

namespace FML
{

	class DebugDraw
	{
	public:
		static void DrawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color);
		static void DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color);
		static void DrawRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawPoint(const glm::vec2& position, const glm::vec4& color);
		static void DrawForwardVector(SDL_Renderer* renderer, const glm::vec2& position, float rotationDegrees, float length = 25.0f);

		static void Render(SDL_Renderer* renderer);

	private:
		struct Line { glm::vec2 start, end; glm::vec4 color; };
		struct Circle { glm::vec2 center; float radius; glm::vec4 color; };
		struct Rectangle { glm::vec2 position, size; glm::vec4 color; };
		struct Point { glm::vec2 position; glm::vec4 color; };

		static std::vector<Line> lines;
		static std::vector<Circle> circles;
		static std::vector<Rectangle> rectangles;
		static std::vector<Point> points;
	};

}