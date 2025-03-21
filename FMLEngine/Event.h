#pragma once

class Observer;

class Event 
{
public:
    virtual ~Event() = default;
    virtual void Process(Observer& observer) const = 0;
};
