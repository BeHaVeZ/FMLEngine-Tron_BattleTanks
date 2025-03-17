#pragma once
#include <variant>

enum class EventType 
{
    DamageTaken,
    ScoreAdded,
};

struct DamageEvent 
{
    int damageAmount;
};

struct ScoreAddedEvent 
{
    int scoreAmount;
};

using EventData = std::variant<DamageEvent, ScoreAddedEvent>;

class Event 
{
public:
    EventType type;
    EventData data;

    static Event Damage(int amount) 
    {
        return Event{ EventType::DamageTaken, DamageEvent{amount} };
    }

    static Event AddScore(int amount) 
    {
        return Event{ EventType::ScoreAdded, ScoreAddedEvent{amount} };
    }
};
