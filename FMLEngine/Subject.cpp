#include "Subject.h"
#include "Observer.h"
#include <algorithm>

namespace FML
{
void Subject::AddObserver(Observer* observer)
{
	if (observer && std::find(observers.begin(), observers.end(), observer) == observers.end())
	{
		observers.emplace_back(observer);
	}
}

void Subject::RemoveObserver(Observer* observer)
{
	observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Subject::Notify(const Event& event)
{
	const auto snapshot = observers;
	for (auto* observer : snapshot)
	{
		if (observer && std::find(observers.begin(), observers.end(), observer) != observers.end())
		{
			observer->HandleEvent(event);
		}
	}
}
}
