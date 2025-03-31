#pragma once
#include "Event.h"

namespace FML 
{
    class Observer {
    public:
        virtual ~Observer() {}
        virtual void HandleEvent(const Event& event) = 0;
    };
}
