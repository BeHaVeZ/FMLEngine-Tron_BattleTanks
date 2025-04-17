#pragma once
#include <memory>
#include <unordered_map>
#include "GameObject.h"
#include "TextureComponent.h"
#include "TransformComponent.h"

namespace FML
{

	class PrefabRegistry
	{
	public:
		static PrefabRegistry& Instance();
		std::unique_ptr<GameObject> CreateRedTankPrefab(glm::vec2 spawnPosition = { 0,0 }, const std::string tag = "") const;
		std::unique_ptr<GameObject> CreateBlueTankPrefab(glm::vec2 spawnPosition = { 0,0 }, const std::string tag = "") const;
		std::unique_ptr<GameObject> CreatePinkTankPrefab(glm::vec2 spawnPosition, const std::string tag) const;
		std::unique_ptr<GameObject> CreateYellowTankPrefab(glm::vec2 spawnPosition = { 0,0 }, const std::string tag = "") const;
		std::unique_ptr<GameObject> CreateBulletPrefab(glm::vec2 spawnPosition = { 0,0 }, glm::vec2 moveDirection = { 0,-1 }, const std::string tag = "") const;

		std::unique_ptr<GameObject> CreateHealthUIForPlayer1(glm::vec2 spawnPosition = { 0,0 }, int maxHealth = 3, const std::string tag = "HealthUIPlayer1") const;

		std::unique_ptr<GameObject> CreateHealthUIForPlayer2(glm::vec2 spawnPosition = { 0,0 }, int maxHealth = 3, const std::string tag = "HealthUIPlayer2") const;

		std::unique_ptr<GameObject> CreateHighScoreUI(glm::vec2 spawnPosition = {0,0}, const std::string tag = "HighScoreUI") const;

		std::unique_ptr<GameObject> CreateCurrentScoreUI(glm::vec2 spawnPosition = { 0,0 }, const std::string tag = "CurrentScoreUI") const;

	private:
		PrefabRegistry() {}
		PrefabRegistry(const PrefabRegistry&) = delete;
		PrefabRegistry& operator=(const PrefabRegistry&) = delete;
	};

}
