#pragma once
#include <variant>

enum class EventType 
{
    DamageTaken
};

struct DamageEvent 
{
    int damageAmount;
};

using EventData = std::variant<DamageEvent>;

class Event
{
public:
    EventType type;
    EventData data;

    static Event Damage(int amount) 
    {
        return Event{ EventType::DamageTaken, DamageEvent{amount} };
    }
};
