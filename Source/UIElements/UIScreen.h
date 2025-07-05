#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../Math.h"
#include "UIText.h"
#include "UIButton.h"
#include "UIImage.h"

class UIScreen
{
public:
    // Tracks if the UI is active or closing
    enum class UIState
    {
        Active,
        Closing
    };

	UIScreen(class Game* game, const std::string& fontName);
	virtual ~UIScreen();

	// UIScreen subclasses can override these
	virtual void Update(float deltaTime);
	virtual void Draw(class SDL_Renderer *renderer);
	virtual void ProcessInput(const uint8_t* keys);
	virtual void HandleKeyPress(int key, int controllerButton, int controllerAxisY);
	virtual void HandleMouse(const SDL_Event& event);

	void SetPosition(Vector2 position) { mPos = position; }
	void SetSize(Vector2 size) { mSize = size; }
	Vector2 GetPosition() const { return mPos; }
	Vector2 GetSize() const { return mSize; }

    // Set state to closing
	void Close();

    // Get state of UI screen
	UIState GetState() const { return mState; }

	void SetIsVisible(bool isVisible) { mIsVisible = isVisible; }

    // Game getter
    class Game* GetGame() const { return mGame; }

	// Buttons getter
	std::vector<UIButton *> GetButtons() { return mButtons; }

	// Texts getter
	std::vector<UIText *> GetTexts() { return mTexts; }

    // Add a button to this screen
	UIButton* AddButton(const std::string& name, const Vector2& pos, const Vector2& dims, const int pointSize, UIButton::TextPos alignText, std::function<void()> onClick, Vector2 textPos = Vector2::Zero, Vector3 textColor = Color::White);
    UIText* AddText(const std::string& name, const Vector2& pos = Vector2::Zero, const Vector2& dims = Vector2::Zero, const int pointSize = 40, Vector3 color = Color::White, const int unsigned wrapLength = 1024);
    UIImage* AddImage(const std::string& imagePath, const Vector2& pos, const Vector2& dims, const Vector3& color = Color::White);

	virtual void ChangeResolution(float oldScale, float newScale);
protected:
    // Sets the mouse mode to relative or not
	class Game* mGame;
	class UIFont* mFont;

	// Configure positions
	Vector2 mPos;
	Vector2 mSize;

	// State
	UIState mState;

	bool mIsVisible;

	// List of buttons, texts, and images
    int mSelectedButtonIndex;
	std::vector<UIButton *> mButtons;
    std::vector<UIText *> mTexts;
    std::vector<UIImage *> mImages;
};
