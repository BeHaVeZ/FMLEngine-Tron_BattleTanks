#pragma once
#include <vector>

namespace FML
{

	class Observer;
	class GameObject;
	class Event;

	class Subject
	{
	private:
		std::vector<Observer*> observers;
	public:
		void AddObserver(Observer* observer);
		void RemoveObserver(Observer* observer);
		void Notify(const Event& event);
	};

}

