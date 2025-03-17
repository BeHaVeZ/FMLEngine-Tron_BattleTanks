#include "Subject.h"

void Subject::AddObserver(Observer* observer)
{
	observers.push_back(observer);
}

void Subject::RemoveObserver(Observer* observer)
{
	observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Subject::Notify(const GameObject& gameObject, Event event)
{
	for (auto observer : observers) 
	{
		observer->OnNotify(gameObject, event);
	}
}
