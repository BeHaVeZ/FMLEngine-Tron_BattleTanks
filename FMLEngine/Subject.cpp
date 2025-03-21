#include "Subject.h"
#include "Observer.h"

void Subject::AddObserver(Observer* observer)
{
	observers.emplace_back(observer);
}

void Subject::RemoveObserver(Observer* observer)
{
	observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Subject::Notify(const Event& event)
{
    for (auto observer : observers) 
    {
        event.Process(*observer);
    }
}
