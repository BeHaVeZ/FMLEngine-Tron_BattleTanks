#include "GodMode.h"
#include "GameObject.h"
#include "GameTags.h"
#include "HealthComponent.h"
#include "Logger.h"
#include "Scene.h"
#include "SceneManager.h"
#include <string>

namespace FML
{
	namespace
	{
		bool enabled = false;

		void ApplyToTag(Scene* scene, const std::string_view tag)
		{
			if (!scene)
				return;

			GodMode::Apply(scene->FindGameObjectByTag(std::string(tag)));
		}
	}

	bool GodMode::IsEnabled()
	{
		return enabled;
	}

	void GodMode::Apply(GameObject* player)
	{
		if (!player)
			return;

		if (auto* health = player->GetComponent<HealthComponent>())
			health->SetInvulnerable(enabled);
	}

	void GodMode::ApplyToCurrentScene()
	{
		Scene* scene = SceneManager::Instance().GetCurrentScene();
		ApplyToTag(scene, Tags::Player1);
		ApplyToTag(scene, Tags::Player2);
	}

	void GodMode::Toggle()
	{
		enabled = !enabled;
		ApplyToCurrentScene();

		Logger::Log(LogLevel::Info, enabled ? "God mode ON" : "God mode OFF");
	}
}
