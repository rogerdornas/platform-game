//
// Created by roger on 30/06/2025.
//

#pragma once

#include <string>
#include "UIElements/UIScreen.h"

class DialogueSystem : public UIScreen
{
public:
    struct DialogueLine {
        std::string speakerName;
        std::string text;
        Vector3 color;
    };

    DialogueSystem(class Game* game, const std::string& fontName, std::string dialogueFilePath);

private:
    void LoadDialogueFile(const std::string& dialogueFilePath);
    void Advance();
    void Finished();
    UIText* mDialogueText;
    std::vector<DialogueLine> mLines;
    int mCurrentLine;
};

