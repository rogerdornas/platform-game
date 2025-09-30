//
// Created by roger on 29/09/2025.
//

#include "SaveManager.h"
#include "SaveData.h"
#include <iostream>

SaveManager::SaveManager(class Game *game)
    :mGame(game)
    ,mSaveFolder("../Saves")
{
}

std::string SaveManager::GetSlotFilename(int slot) {
    return mSaveFolder + "/slot_" + std::to_string(slot) + ".json";
}

void SaveManager::SaveGame(SaveData* data, int slot) {
    std::string filename = GetSlotFilename(slot);
    data->Save(filename);
}

SaveData* SaveManager::LoadGame(int slot) {
    std::string filename = GetSlotFilename(slot);
    auto* data = new SaveData(mGame);
    data->Load(filename);
    return data;
}

void SaveManager::DeleteSave(int slot) {
    std::string filename = GetSlotFilename(slot);
    auto* data = new SaveData(mGame);
    data->Save(filename);
}
