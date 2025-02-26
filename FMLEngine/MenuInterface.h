#pragma once

class MenuInterface {
public:
    virtual ~MenuInterface() = default;

    virtual void SelectTopItem() = 0;
    virtual void SelectLeftItem() = 0;
    virtual void SelectRightItem() = 0;
    virtual void SelectBottomItem() = 0;
};