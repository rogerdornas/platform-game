#pragma once

#include <vector>
#include <SDL_stdinc.h>
#include "../Math.h"
#include <SDL.h>

enum class ActorState { Active, Paused, Destroy };

class Actor
{
public:
    Actor(class Game* game);
    virtual ~Actor();

    // Update a function called from Game (not overridable)
    void Update(float deltaTime);
    // ProcessInput function called from Game (not overridable)
    void ProcessInput(const Uint8* keyState, SDL_GameController& controller);

    // Position getter/setter
    const Vector2& GetPosition() const { return mPosition; }
    void SetPosition(const Vector2& pos) { mPosition = pos; }

    Vector2 GetForward() const { return Vector2(Math::Cos(mRotation), Math::Sin(mRotation)); }

    // Scale getter/setter
    float GetScale() const { return mScale; }
    void SetScale(const float scale) { mScale = scale; }

    virtual void ChangeResolution(float oldScale, float newScale);

    // Rotation getter/setter
    float GetRotation() const { return mRotation; }
    void SetRotation(float rotation) { mRotation = rotation; }

    // State getter/setter
    ActorState GetState() const { return mState; }
    void SetState(ActorState state) { mState = state; }

    virtual float GetWidth() { return -1.0f; }
    virtual float GetHeight(){ return -1.0f; }

    // Game getter
    class Game* GetGame() const { return mGame; }

    // Returns a component of type T, or null if doesn't exist
    template<typename T>
    T* GetComponent() const {
        for (auto c: mComponents) {
            T *t = dynamic_cast<T*>(c);
            if (t != nullptr) {
                return t;
            }
        }
        return nullptr;
    }

    void RemoveComponent(const class Component* c);

protected:
    class Game* mGame;

    // Any actor-specific update code (overridable)
    virtual void OnUpdate(float deltaTime);
    // Any actor-specific update code (overridable)
    virtual void OnProcessInput(const Uint8 *keyState, SDL_GameController &controller);

    // Actor's state
    ActorState mState;

    // Transform
    Vector2 mPosition;
    float mScale;
    float mRotation;

    // Components
    std::vector<class Component *> mComponents;

private:
    friend class Component;

    // Adds component to Actor (this is automatically called
    // in the component constructor)
    void AddComponent(class Component *c);
};
