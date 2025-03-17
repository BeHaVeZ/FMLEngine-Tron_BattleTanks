#pragma once
#include <string>

class Event
{
public:
	Event() = default;
	Event(const std::string& eventName)
	{
		m_EventName = eventName;
	}
	virtual ~Event() = default;

	const std::string& GetName() const
	{
		return m_EventName;
	}
private:
	std::string m_EventName{};
};