#pragma once
#include "Command.h"
#include "Logger.h"
#include "PrefabRegistry.h"

namespace FML
{
	class TestCommand : public Command
	{
	public:
		TestCommand();
		~TestCommand();

		void Execute() override;

	private:
	};

	inline TestCommand::TestCommand()
	{
	}

	inline TestCommand::~TestCommand()
	{
	}
	inline void TestCommand::Execute()
	{
		Logger::Log(LogLevel::Warning, "TestCommand executed");

		auto tank = PrefabRegistry::Instance().CreateBlueTankPrefab({}, "Enemy");
		SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(tank), { 500,500 });
	}
}