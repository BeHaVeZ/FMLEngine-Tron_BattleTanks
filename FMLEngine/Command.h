#pragma once

namespace FML
{
	class Command {
	public:
		virtual ~Command() {}
		virtual void Execute() = 0;
	};
}