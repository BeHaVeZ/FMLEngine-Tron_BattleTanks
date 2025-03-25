#pragma once
#include "Command.h"
#include "Logger.h"

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
		Logger::Log(LogLevel::Info, "TestCommand executed");
		Logger::Log(LogLevel::Warning, "TestCommand executed");
		Logger::Log(LogLevel::Error, "TestCommand executed");
		Logger::Log(LogLevel::Debug, "TestCommand executed");
	}
}