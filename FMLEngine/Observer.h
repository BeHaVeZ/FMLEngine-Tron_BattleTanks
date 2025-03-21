#pragma once
#include "DamageEvent.h"

class Observer 
{
public:
    virtual ~Observer() {}
    virtual void HandleEvent(const DamageEvent& event) = 0;
};
