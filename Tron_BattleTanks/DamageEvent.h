#pragma once
#include "Event.h"

namespace FML
{

	class DamageEvent final : public Event
	{
	public:
		DamageEvent(int damage) : damageAmount(damage) {}
		int GetDamage() const { return damageAmount; }

	private:
		int damageAmount;
	};

}
