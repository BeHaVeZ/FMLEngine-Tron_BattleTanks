#pragma once
#include "Event.h"

class DamageEvent final : public Event 
{
public:
    DamageEvent(int damage) : damageAmount(damage) {}
    void Process(Observer& observer) const override;
    int GetDamage() const;

private:
    int damageAmount;
};
