#include "DamageEvent.h"
#include "Observer.h"

namespace FML
{

	void DamageEvent::Process(Observer& observer) const
	{
		observer.HandleEvent(*this);
	}

	int DamageEvent::GetDamage() const
	{
		return damageAmount;
	}

}
