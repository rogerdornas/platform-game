//
// Created by roger on 29/09/2025.
//

#pragma once
#include "Game.h"

class SaveManager
{
public:
    SaveManager(class Game* game);

    void SaveGame(class SaveData* data, int slot);
    class SaveData* LoadGame(int slot);
    void DeleteSave(int slot);


private:
    std::string GetSlotFilename(int slot);

    class Game* mGame;
    std::string mSaveFolder;
};
