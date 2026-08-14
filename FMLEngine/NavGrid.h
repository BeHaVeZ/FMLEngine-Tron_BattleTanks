#pragma once
#include <SDL.h>
#include <glm.hpp>
#include <queue>
#include <vector>

namespace FML
{
	class NavGrid
	{
	public:
		static NavGrid& Instance()
		{
			static NavGrid instance;
			return instance;
		}

		NavGrid(const NavGrid&) = delete;
		NavGrid& operator=(const NavGrid&) = delete;

		void Build(const std::vector<SDL_Rect>& blockers, const SDL_Rect& bounds, int cellSize);
		void Clear();

		bool IsBuilt() const { return width > 0 && height > 0; }
		int GetCellSize() const { return cellSize; }

		bool FindPath(const glm::vec2& start, const glm::vec2& goal, float agentRadius, std::vector<glm::vec2>& outPath);

		bool IsWalkable(const glm::vec2& worldPosition) const;

		int GetLastSearchedCellCount() const { return lastSearchedCells; }
		int GetSearchCount() const { return searchCount; }
		int ConsumeSearchedCellsSinceLastFrame();

		void DebugRenderGrid() const;
		void DebugRenderPath(const std::vector<glm::vec2>& path, size_t nextWaypoint) const;

	private:
		NavGrid() = default;

		struct Cell
		{
			int x{ 0 };
			int y{ 0 };
		};

		bool InBounds(Cell cell) const { return cell.x >= 0 && cell.y >= 0 && cell.x < width && cell.y < height; }
		int Index(Cell cell) const { return cell.y * width + cell.x; }
		Cell ToCell(const glm::vec2& worldPosition) const;
		glm::vec2 ToWorldCenter(Cell cell) const;

		void BuildClearanceField();
		bool FindNearestUsableCell(Cell from, int requiredClearance, Cell& outCell) const;
		bool Search(Cell start, Cell goal, int requiredClearance);
		void BuildWorldPath(Cell start, Cell goal, std::vector<glm::vec2>& outPath) const;

		int width{ 0 };
		int height{ 0 };
		int cellSize{ 0 };
		glm::ivec2 origin{ 0, 0 };

		std::vector<uint8_t> blocked;
		std::vector<uint8_t> clearance;

		struct OpenNode
		{
			float estimatedTotalCost{ 0.f };
			int index{ 0 };
			bool operator>(const OpenNode& other) const { return estimatedTotalCost > other.estimatedTotalCost; }
		};

		std::vector<float> costFromStart;
		std::vector<int> cameFrom;
		std::vector<uint8_t> closed;
		std::priority_queue<OpenNode, std::vector<OpenNode>, std::greater<OpenNode>> open;
		int lastSearchedCells{ 0 };
		int searchCount{ 0 };
		int searchedCellsAccumulator{ 0 };
	};
}
