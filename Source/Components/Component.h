#pragma once

#include <SDL_stdinc.h>

class Component
{
public:
    // Constructor
    // (the lower the update order, the earlier the component updates)
    Component(class Actor* owner, int updateOrder = 100);
    virtual ~Component();

    virtual void Update(float deltaTime);

    // Process input for this component (if needed)
    virtual void ProcessInput(const Uint8* keyState);

    int GetUpdateOrder() const { return mUpdateOrder; }
    class Actor* GetOwner() const { return mOwner; }
    class Game* GetGame() const;

protected:
    // Owning actor
    class Actor* mOwner;
    // Update order
    int mUpdateOrder;
};
