#pragma once

class MenuInterface {
public:
    virtual ~MenuInterface() = default;

    virtual void MoveSelectionUp() = 0;
    virtual void MoveSelectionDown() = 0;
    virtual void SelectOption() = 0;
};