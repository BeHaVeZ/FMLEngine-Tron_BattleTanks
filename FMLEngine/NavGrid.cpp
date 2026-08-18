#include "NavGrid.h"
#include "DebugDraw.h"
#include "DebugOverlay.h"
#include "Logger.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <utility>

namespace FML
{
	namespace
	{
		constexpr int invalidIndex = -1;
		constexpr uint8_t maxClearance = 255;
		constexpr int maxRandomGoalAttempts = 64;

		constexpr float turnPenalty = .8f;
		constexpr int preferredClearance = 5;
		constexpr float centringWeight = .2f;

		constexpr float occupancyPenalty = 8.f;
	}

	void NavGrid::Build(const std::vector<SDL_Rect>& blockers, const SDL_Rect& bounds, int newCellSize)
	{
		Clear();

		if (newCellSize <= 0 || bounds.w <= 0 || bounds.h <= 0)
		{
			Logger::Log(LogLevel::Error, "NavGrid::Build called with invalid bounds or cell size");
			return;
		}

		cellSize = newCellSize;
		origin = { bounds.x, bounds.y };
		width = (bounds.w + cellSize - 1) / cellSize;
		height = (bounds.h + cellSize - 1) / cellSize;

		blocked.assign(static_cast<size_t>(width) * height, 0);

		for (const SDL_Rect& blocker : blockers)
		{
			const int minX = std::max(0, (blocker.x - origin.x) / cellSize);
			const int minY = std::max(0, (blocker.y - origin.y) / cellSize);
			const int maxX = std::min(width - 1, (blocker.x + blocker.w - 1 - origin.x) / cellSize);
			const int maxY = std::min(height - 1, (blocker.y + blocker.h - 1 - origin.y) / cellSize);

			for (int y = minY; y <= maxY; ++y)
			{
				for (int x = minX; x <= maxX; ++x)
				{
					blocked[static_cast<size_t>(y) * width + x] = 1;
				}
			}
		}

		BuildClearanceField();

		costFromStart.resize(blocked.size());
		cameFrom.resize(blocked.size());
		closed.resize(blocked.size());
		occupancy.assign(blocked.size(), 0.f);

		Logger::Log(LogLevel::Info, "NavGrid built: %dx%d cells at %dpx from %zu blockers",
			width, height, cellSize, blockers.size());
	}

	void NavGrid::Clear()
	{
		width = 0;
		height = 0;
		cellSize = 0;
		origin = { 0, 0 };
		blocked.clear();
		clearance.clear();
		occupancy.clear();
		costFromStart.clear();
		cameFrom.clear();
		closed.clear();
		lastSearchedCells = 0;
	}

	void NavGrid::BuildClearanceField()
	{
		clearance.assign(blocked.size(), 0);

		for (size_t i = 0; i < blocked.size(); ++i)
		{
			clearance[i] = blocked[i] ? 0 : maxClearance;
		}

		const auto relax = [this](int x, int y, int neighbourX, int neighbourY)
			{
				if (neighbourX < 0 || neighbourY < 0 || neighbourX >= width || neighbourY >= height)
					return;

				const uint8_t candidate = static_cast<uint8_t>(std::min<int>(maxClearance, clearance[static_cast<size_t>(neighbourY) * width + neighbourX] + 1));
				uint8_t& target = clearance[static_cast<size_t>(y) * width + x];
				target = std::min(target, candidate);
			};

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				relax(x, y, x - 1, y);
				relax(x, y, x, y - 1);
				relax(x, y, x - 1, y - 1);
				relax(x, y, x + 1, y - 1);
			}
		}

		for (int y = height - 1; y >= 0; --y)
		{
			for (int x = width - 1; x >= 0; --x)
			{
				relax(x, y, x + 1, y);
				relax(x, y, x, y + 1);
				relax(x, y, x + 1, y + 1);
				relax(x, y, x - 1, y + 1);
			}
		}
	}

	void NavGrid::BuildOccupancyField(const std::vector<glm::vec2>& occupied, float radius)
	{
		std::fill(occupancy.begin(), occupancy.end(), 0.f);

		const int reach = static_cast<int>(std::ceil(radius / cellSize));
		const float radiusSquared = radius * radius;

		for (const glm::vec2& centre : occupied)
		{
			const Cell cell = ToCell(centre);

			for (int y = cell.y - reach; y <= cell.y + reach; ++y)
			{
				for (int x = cell.x - reach; x <= cell.x + reach; ++x)
				{
					const Cell candidate{ x, y };
					if (!InBounds(candidate))
						continue;

					const glm::vec2 offset = ToWorldCenter(candidate) - centre;
					if (offset.x * offset.x + offset.y * offset.y > radiusSquared)
						continue;

					occupancy[Index(candidate)] += occupancyPenalty;
				}
			}
		}
	}

	NavGrid::Cell NavGrid::ToCell(const glm::vec2& worldPosition) const
	{
		return {
			static_cast<int>(std::floor((worldPosition.x - origin.x) / static_cast<float>(cellSize))),
			static_cast<int>(std::floor((worldPosition.y - origin.y) / static_cast<float>(cellSize)))
		};
	}

	glm::vec2 NavGrid::ToWorldCenter(Cell cell) const
	{
		return {
			origin.x + (cell.x + 0.5f) * cellSize,
			origin.y + (cell.y + 0.5f) * cellSize
		};
	}

	bool NavGrid::IsWalkable(const glm::vec2& worldPosition) const
	{
		if (!IsBuilt())
			return true;

		const Cell cell = ToCell(worldPosition);
		return InBounds(cell) && !blocked[Index(cell)];
	}

	int NavGrid::ClearanceForRadius(float agentRadius) const
	{
		return std::max(1, static_cast<int>(std::ceil(agentRadius / cellSize + 0.5f)));
	}

	bool NavGrid::FindRandomGoal(const glm::vec2& from, float agentRadius, float minDistance, glm::vec2& outGoal)
	{
		if (!IsBuilt())
			return false;

		const int requiredClearance = ClearanceForRadius(agentRadius);
		const float minDistanceSquared = minDistance * minDistance;

		std::uniform_int_distribution<int> xDistribution(0, width - 1);
		std::uniform_int_distribution<int> yDistribution(0, height - 1);

		for (int attempt = 0; attempt < maxRandomGoalAttempts; ++attempt)
		{
			const Cell candidate{ xDistribution(rng), yDistribution(rng) };
			const int index = Index(candidate);
			if (blocked[index] || clearance[index] < requiredClearance)
				continue;

			const glm::vec2 world = ToWorldCenter(candidate);
			const glm::vec2 offset = world - from;
			if (offset.x * offset.x + offset.y * offset.y < minDistanceSquared)
				continue;

			outGoal = world;
			return true;
		}

		return false;
	}

	bool NavGrid::FindNearestUsableCell(Cell from, int requiredClearance, Cell& outCell) const
	{
		const auto usable = [&](Cell cell)
			{
				return InBounds(cell) && !blocked[Index(cell)] && clearance[Index(cell)] >= requiredClearance;
			};

		if (usable(from))
		{
			outCell = from;
			return true;
		}

		constexpr int maxRingRadius = 12;
		for (int radius = 1; radius <= maxRingRadius; ++radius)
		{
			for (int offset = -radius; offset <= radius; ++offset)
			{
				const std::array<Cell, 4> ring{
					Cell{ from.x + offset, from.y - radius },
					Cell{ from.x + offset, from.y + radius },
					Cell{ from.x - radius, from.y + offset },
					Cell{ from.x + radius, from.y + offset }
				};

				for (const Cell& candidate : ring)
				{
					if (usable(candidate))
					{
						outCell = candidate;
						return true;
					}
				}
			}
		}

		return false;
	}

	bool NavGrid::Search(Cell start, Cell goal, int requiredClearance)
	{
		std::fill(costFromStart.begin(), costFromStart.end(), std::numeric_limits<float>::max());
		std::fill(cameFrom.begin(), cameFrom.end(), invalidIndex);
		std::fill(closed.begin(), closed.end(), uint8_t{ 0 });
		open = {};
		lastSearchedCells = 0;

		const int startIndex = Index(start);
		const int goalIndex = Index(goal);

		const auto heuristic = [&](Cell cell)
			{
				return static_cast<float>(std::abs(cell.x - goal.x) + std::abs(cell.y - goal.y));
			};

		const auto centringCost = [&](int index)
			{
				const int headroom = clearance[index];
				return headroom >= preferredClearance ? 0.f : (preferredClearance - headroom) * centringWeight;
			};

		costFromStart[startIndex] = 0.f;
		open.push({ heuristic(start), startIndex });

		static constexpr std::array<Cell, 4> neighbourOffsets{
			Cell{ 1, 0 }, Cell{ -1, 0 }, Cell{ 0, 1 }, Cell{ 0, -1 }
		};

		while (!open.empty())
		{
			const int current = open.top().index;
			open.pop();

			if (closed[current])
				continue;

			closed[current] = 1;
			++lastSearchedCells;
			++searchedCellsAccumulator;

			if (current == goalIndex)
				return true;

			const Cell currentCell{ current % width, current / width };

			const int parent = cameFrom[current];
			const Cell incoming = (parent == invalidIndex)
				? Cell{ 0, 0 }
				: Cell{ currentCell.x - parent % width, currentCell.y - parent / width };

			for (const Cell& offset : neighbourOffsets)
			{
				const Cell neighbour{ currentCell.x + offset.x, currentCell.y + offset.y };
				if (!InBounds(neighbour))
					continue;

				const int neighbourIndex = Index(neighbour);
				if (blocked[neighbourIndex] || closed[neighbourIndex] || clearance[neighbourIndex] < requiredClearance)
					continue;

				const bool turning = parent != invalidIndex && (offset.x != incoming.x || offset.y != incoming.y);

				const float tentativeCost = costFromStart[current]
					+ 1.f
					+ (turning ? turnPenalty : 0.f)
					+ centringCost(neighbourIndex)
					+ occupancy[neighbourIndex];
				if (tentativeCost >= costFromStart[neighbourIndex])
					continue;

				costFromStart[neighbourIndex] = tentativeCost;
				cameFrom[neighbourIndex] = current;
				open.push({ tentativeCost + heuristic(neighbour), neighbourIndex });
			}
		}

		return false;
	}

	void NavGrid::BuildWorldPath(Cell start, Cell goal, std::vector<glm::vec2>& outPath) const
	{
		std::vector<Cell> reversed;
		for (int index = Index(goal); index != invalidIndex; index = cameFrom[index])
		{
			reversed.push_back({ index % width, index / width });
			if (index == Index(start))
				break;
		}

		std::reverse(reversed.begin(), reversed.end());

		for (size_t i = 1; i < reversed.size(); ++i)
		{
			const bool isLast = (i + 1 == reversed.size());
			if (!isLast)
			{
				const Cell& previous = reversed[i - 1];
				const Cell& current = reversed[i];
				const Cell& next = reversed[i + 1];

				const bool collinear = (next.x - current.x) == (current.x - previous.x)
					&& (next.y - current.y) == (current.y - previous.y);
				if (collinear)
					continue;
			}

			outPath.push_back(ToWorldCenter(reversed[i]));
		}
	}

	bool NavGrid::FindPath(const glm::vec2& start, const glm::vec2& goal, float agentRadius, std::vector<glm::vec2>& outPath,
		const std::vector<glm::vec2>& occupied)
	{
		outPath.clear();

		if (!IsBuilt())
			return false;

		++searchCount;

		BuildOccupancyField(occupied, agentRadius * 2.f);

		const int desiredClearance = ClearanceForRadius(agentRadius);

		for (int requiredClearance = desiredClearance; requiredClearance >= 1; --requiredClearance)
		{
			Cell startCell{};
			Cell goalCell{};
			if (!FindNearestUsableCell(ToCell(start), requiredClearance, startCell))
				continue;
			if (!FindNearestUsableCell(ToCell(goal), requiredClearance, goalCell))
				continue;

			if (Index(startCell) == Index(goalCell))
				return false;

			if (Search(startCell, goalCell, requiredClearance))
			{
				BuildWorldPath(startCell, goalCell, outPath);
				return !outPath.empty();
			}
		}

		return false;
	}

	int NavGrid::ConsumeSearchedCellsSinceLastFrame()
	{
		return std::exchange(searchedCellsAccumulator, 0);
	}

	void NavGrid::DebugRenderGrid() const
	{
		if (!IsBuilt())
			return;

		const bool showBlocked = DebugEnabled(DebugChannel::NavGrid);
		const bool showClearance = DebugEnabled(DebugChannel::Clearance);
		if (!showBlocked && !showClearance)
			return;

		const glm::vec2 cellExtent{ static_cast<float>(cellSize), static_cast<float>(cellSize) };
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				const size_t index = static_cast<size_t>(y) * width + x;
				const glm::vec2 cellOrigin{ static_cast<float>(origin.x + x * cellSize), static_cast<float>(origin.y + y * cellSize) };

				if (blocked[index])
				{
					if (showBlocked)
						DebugDraw::DrawFilledRectangle(cellOrigin, cellExtent, { .9f, .15f, .15f, .35f });
					continue;
				}

				if (!showClearance)
					continue;

				constexpr float clearanceScale = 5.f;
				const float t = std::min(1.f, clearance[index] / clearanceScale);
				DebugDraw::DrawFilledRectangle(cellOrigin, cellExtent, { .1f, .25f + .55f * t, 1.f - .8f * t, .30f });
			}
		}
	}

	void NavGrid::DebugRenderPath(const std::vector<glm::vec2>& path, size_t nextWaypoint) const
	{
		if (!DebugEnabled(DebugChannel::Paths))
			return;

		for (size_t i = 1; i < path.size(); ++i)
		{
			DebugDraw::DrawLine(path[i - 1], path[i], { 0.f, 1.f, 1.f, .9f });
		}

		for (size_t i = 0; i < path.size(); ++i)
		{
			const glm::vec4 colour = (i < nextWaypoint) ? glm::vec4{ 0.f, .4f, .45f, .8f } : glm::vec4{ 0.f, 1.f, 1.f, 1.f };
			DebugDraw::DrawCircle(path[i], (i == nextWaypoint) ? 6.f : 3.f, colour);
		}
	}
}
