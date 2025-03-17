#pragma once
#include "memory"
#include "Observer.h"
#include "GameObject.h"


class Subject {
private:
    std::vector<Observer*> observers;

public:
    void AddObserver(Observer* observer);

    void RemoveObserver(Observer* observer);

    void Notify(const GameObject& gameObject, Event event);
};
