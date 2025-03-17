#pragma once
#include "Observer.h"
#include "Event.h"
#include "AddScoreEvent.h"
#include "TextComponent.h"
#include "Subject.h"

class ScoreComponent : public Component, public Observer<AddScoreEvent>
{
public:
    ScoreComponent();
    virtual ~ScoreComponent() = default;

    void Initialize() override;

	void AddObserver(Observer<AddScoreEvent>* observer) 
    {
		subject->AddObserver(observer);
	}
	void RemoveObserver(Observer<AddScoreEvent>* observer)
	{
		subject->RemoveObserver(observer);
	}

private:
    int score = 0;
	std::unique_ptr<Subject<AddScoreEvent>> subject;
};