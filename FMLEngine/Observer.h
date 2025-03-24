#pragma once
#include "DamageEvent.h"

namespace FML
{
	class Observer
	{
	public:
		virtual ~Observer() {}
		virtual void HandleEvent(const DamageEvent& event) = 0;
	};
}