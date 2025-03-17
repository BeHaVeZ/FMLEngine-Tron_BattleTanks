#pragma once
#include "Observer.h"
#include <vector>


template <typename T>
class Subject final
{
public:
	Subject() = default;

	void AddObserver(Observer<T>* pObserver)
	{
		observers.push_back(pObserver);
	}

	void RemoveObserver(Observer<T>* pObserver)
	{
		observers.erase(std::remove(observers.begin(), observers.end(), pObserver), observers.end());
	}

	void NotifyObservers(T* data)
	{
		for (auto observer : observers)
		{
			observer->OnNotify(data);
		}
	}

private:
	std::vector<Observer<T>*> observers{};
};