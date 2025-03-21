#include "DamageEvent.h"
#include "Observer.h"

void DamageEvent::Process(Observer& observer) const
{
	observer.HandleEvent(*this);
}

int DamageEvent::GetDamage() const 
{
	return damageAmount;
}