#include "Inventory.h"
#include "Audio/Audio.h"
#include "Core/Game.h"
#include "Input/Input.h"

void Inventory::Update(float deltaTime) {
    if (m_state == InventoryState::ITEM_VIEW_SCREEN) UpdateItemViewScreen();
    if (m_state == InventoryState::ITEM_EXAMINE) UpdateExamineScreen();
}


void Inventory::UpdateItemViewScreen() {
    Player* player = Game::GetLocalPlayerByIndex(m_localPlayerIndex);
    if (!player) return;

    // WASD cell input
    if (player->PressedLeft()) {
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
        m_selectedCellX--;
    }
    if (player->PressedRight()) {
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
        m_selectedCellX++;
    }
    if (player->PressedUp()) {
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
        m_selectedCellY--;
    }
    if (player->PressedDown()) {
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
        m_selectedCellY++;
    }

    // Wrap check Left
    if (m_selectedCellX >= m_gridCountX) {
        m_selectedCellX = 0;
    }
    // Wrap check Right
    if (m_selectedCellX < 0) {
        m_selectedCellX = m_gridCountX - 1;
    }
    // Wrap check bottom
    if (m_selectedCellY >= MAX_INVENTORY_Y_SIZE) {
        m_selectedCellY = 0;
    }
    // Wrap check top
    if (m_selectedCellY < 0) {
        m_selectedCellY = MAX_INVENTORY_Y_SIZE - 1;
    }

    // Buttons
    if (player->PressedInventoryExamine()) {
        SetState(InventoryState::ITEM_EXAMINE);
    }
}

void Inventory::UpdateExamineScreen() {
    Player* player = Game::GetLocalPlayerByIndex(m_localPlayerIndex);
    if (!player) return;
}