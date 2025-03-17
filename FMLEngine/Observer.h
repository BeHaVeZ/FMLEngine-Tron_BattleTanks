#pragma once

template <typename T>
class Observer
{
public:
	virtual ~Observer() = default;

	virtual void OnNotify(T* event) = 0;
};