#pragma once
#include "HellDefines.h"
#include "HellEnums.h"
#include "HellTypes.h"
#include <string>
#include <vector>

struct InventoryItem {
    std::string m_name = UNDEFINED_STRING;
    glm::ivec2 m_gridLocation = glm::ivec2(-1, -1);
    std::vector<std::string> m_attachments;
    int m_ammoInMag = 0;
    bool m_rotatedInGrid = false;
};

struct Inventory {
    Inventory();
    void Update(float deltaTime);
    void AddItem(const std::string& name);
    void ClearInventory();
    void OpenInventory();
    void CloseInventory();
    void SubmitRenderItems();
    void PrintGridOccupiedStateToConsole();
    void SetLocalPlayerIndex(int localPlayerIndex);
    void SetState(InventoryState state);

    const InventoryState& GetInventoryState()    { return m_state; }
    const std::vector<InventoryItem>& GetItems() { return m_items; }
    const bool IsClosed()                        { return m_state == InventoryState::CLOSED; }
    const bool IsOpen()                          { return m_state != InventoryState::CLOSED; }

private:
    glm::ivec2 GetNextFreeLocation(int itemCellSize);
    void UpdateOccupiedSlotsArray(); // rename thsi to reflect the acutal name of the array: m_itemIndex2DArray
    void RenderButton(glm::ivec2 location, const std::string& letter, const std::string& description);

    // Render submits
    void SubmitItemViewScreenRenderItems();
    void SubmitItemExamineRenderItems();
    
    // Updates
    void UpdateItemViewScreen();
    void UpdateExamineScreen();

    // Getters
    InventoryItemInfo* GetSelectedItemInfo();
    InventoryItem* GetItemAtIndex(int index);
    int GetSelectedItemIndex();
    const std::string& GetItemNameAtLocation(int x, int y);
    int GetItemSizeAtLocation(int x, int y);
    bool IsCellOccupied(int x, int y);

    int m_itemIndex2DArray[MAX_INVENTORY_X_SIZE][MAX_INVENTORY_Y_SIZE]; // Updated on insertion and move
    int m_gridCountX = 4;
    int m_selectedCellX = 0;
    int m_selectedCellY = 0;
    int m_localPlayerIndex = 0;
    std::vector<InventoryItem> m_items;
    InventoryState m_state = InventoryState::CLOSED;
};
