#pragma once
#include "GameObject.h"
#include "Event.h"

class Observer 
{
public:
    virtual ~Observer() {}
    virtual void OnNotify(const GameObject& gameObject, Event event) = 0;
};
