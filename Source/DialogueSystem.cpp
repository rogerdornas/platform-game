//
// Created by roger on 30/06/2025.
//

#include "DialogueSystem.h"
#include "Game.h"
#include "UIElements/UIText.h"
#include <fstream>
#include <sstream>

DialogueSystem::DialogueSystem(class Game *game, const std::string &fontName, std::string dialogueFilePath)
    :UIScreen(game, fontName)
    ,mCurrentLine(0)
{
    LoadDialogueFile(dialogueFilePath);

    SetSize(Vector2(mGame->GetLogicalWindowWidth() * 0.9f, mGame->GetLogicalWindowHeight() * 0.2f));
    SetPosition(Vector2(mGame->GetLogicalWindowWidth() * 0.05f, mGame->GetLogicalWindowHeight() * 0.75f));

    AddImage("../Assets/Sprites/Background/Store.png", Vector2::Zero, GetSize());

    if (!mLines.empty()) {
        int textPointSize = static_cast<int>(40 * mGame->GetScale());
        // std::string text = mLines[mCurrentLine].speakerName + ": " + mLines[mCurrentLine].text;
        std::string text = mLines[mCurrentLine].text;
        Vector3 color = mLines[mCurrentLine].color;
        int wrapLength = GetSize().x * 0.9;

        mDialogueText = AddText(text, GetSize() * 0.05f,
                               Vector2::Zero * mGame->GetScale(),
                                textPointSize, color, wrapLength);

    } else {
        Finished();
    }

    std::string buttonName = "Próximo";
    Vector2 buttonPos = Vector2(GetSize().x * 0.85f, GetSize().y * 0.7f);
    Vector2 buttonSize = Vector2(GetSize().x / 10, 50 * mGame->GetScale());
    int buttonPointSize = static_cast<int>(34 * mGame->GetScale());
    AddButton(buttonName, buttonPos, buttonSize, buttonPointSize, UIButton::TextPos::Center,
        [this]() {
            Advance();
        });

    if (mGame->GetIsPlayingOnKeyboard()) {
        UIText* text = AddText("[ENTER]", Vector2::Zero, Vector2::Zero, static_cast<int>(20 * mGame->GetScale()));
        text->SetPosition(Vector2(GetSize().x * 0.953f, GetSize().y * 0.80f));
    }
    else {
        UIText* text = AddText("[A]", Vector2::Zero, Vector2::Zero, static_cast<int>(20 * mGame->GetScale()));
        text->SetPosition(Vector2(GetSize().x * 0.953f, GetSize().y * 0.80f));
    }

}

void DialogueSystem::LoadDialogueFile(const std::string &dialogueFilePath) {
    std::ifstream file(dialogueFilePath);
    if (!file.is_open()) {
        SDL_Log("Erro ao abrir o arquivo de diálogo: %s", dialogueFilePath.c_str());
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos || colonPos == 0 || colonPos == line.size() - 1) {
            SDL_Log("Linha inválida no diálogo: %s", line.c_str());
            continue;
        }

        std::string speaker = line.substr(0, colonPos);
        std::string text = line.substr(colonPos + 1);

        // Remover espaços extras no início do texto
        text.erase(0, text.find_first_not_of(" \t"));

        // Definir cor do texto
        Vector3 color;
        if (speaker == "Esquilo") {
            color = Vector3(0.65, 0.37, 0.33);
        }
        if (speaker == "Bicho") {
            // color = Vector3(0.25, 0.55, 0.14);
            color = Vector3(0, 1.0, 0);
        }

        mLines.push_back(DialogueLine{ speaker, text, color });
    }

    file.close();
}

void DialogueSystem::Advance() {
    mCurrentLine++;
    if (mCurrentLine < mLines.size()) {
        // Atualiza o texto exibido no UIScreen
        // mDialogueText->SetText(mLines[mCurrentLine].speakerName + ": " + mLines[mCurrentLine].text);
        mDialogueText->SetColor(mLines[mCurrentLine].color);
        mDialogueText->SetText(mLines[mCurrentLine].text);
    } else {
        Finished();
    }
}

void DialogueSystem::Finished() {
    Close();
}
