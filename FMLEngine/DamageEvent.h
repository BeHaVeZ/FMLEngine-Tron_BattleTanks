#pragma once
#include "Event.h"

namespace FML
{

	class DamageEvent final : public Event
	{
	public:
		DamageEvent(int damage) : damageAmount(damage) {}
		void Process(Observer& observer) const override;
		int GetDamage() const;

	private:
		int damageAmount;
	};

}