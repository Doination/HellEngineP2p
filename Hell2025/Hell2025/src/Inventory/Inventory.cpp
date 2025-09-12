#include "Inventory.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Bible.h"
#include "Input/Input.h"
#include "UI/TextBlitter.h"
#include "UI/UiBackend.h"

Inventory::Inventory() {
    //UpdateOccupiedSlotsArray();
}

void Inventory::OpenInventory() {
    m_inventoryState = VIEWING_ITEMS;
    m_selectedCellX = 0;
    m_selectedCellY = 0;
}

void Inventory::CloseInventory() {
    m_inventoryState = CLOSED;
}

void Inventory::AddItem(const std::string& name) {
    InventoryItemInfo* itemInfo = Bible::GetInventoryItemInfoByName(name);
    if (!itemInfo) return;


    std::cout << "\n";
    std::cout << "Adding: " << name << "\n";
    std::cout << "- cell size: " << itemInfo->m_cellSize << "\n";
;   PrintGridOccupiedStateToConsole();

    // Find the next free location
    glm::ivec2 nextFreeLocation = GetNextFreeLocation(itemInfo->m_cellSize);
    
    // Oh there wasn't one?
    if (nextFreeLocation == glm::ivec2(-1, -1)) {
        std::cout << "NO FREE SPACE FOR ITEM '" << name << "'\n";
    }

    std::cout << "- next free location: " << nextFreeLocation.x << ", " << nextFreeLocation.y << "\n";


    // Yay, there was, insert it!
    InventoryItem item;
    item.m_name = name;
    item.m_gridLocation = nextFreeLocation;
    m_items.push_back(item);

    UpdateOccupiedSlotsArray();
}

void Inventory::ClearInventory() {
    m_items.clear();
    UpdateOccupiedSlotsArray();
}

void Inventory::UpdateOccupiedSlotsArray() {
    // Initilize to empty
    for (int x = 0; x < MAX_INVENTORY_X_SIZE; x++) {
        for (int y = 0; y < MAX_INVENTORY_Y_SIZE; y++) {
            m_occupiedSlots[x][y] = false;
        }
    }

    // Iterate your inventory and mark which slots are occupied
    for (InventoryItem& item : m_items) {
        InventoryItemInfo* itemInfo = Bible::GetInventoryItemInfoByName(item.m_name);
        if (!itemInfo) continue;

        m_occupiedSlots[item.m_gridLocation.x][item.m_gridLocation.y] = true;

        if (itemInfo->m_cellSize > 1) {
            m_occupiedSlots[item.m_gridLocation.x + 1][item.m_gridLocation.y] = true;
        }
        if (itemInfo->m_cellSize > 2) {
            m_occupiedSlots[item.m_gridLocation.x + 2][item.m_gridLocation.y] = true;
        }
    }
}

glm::ivec2 Inventory::GetNextFreeLocation(int itemCellSize) {
    UpdateOccupiedSlotsArray();

    // First scan for a non-rotated slot
    for (int y = 0; y < MAX_INVENTORY_Y_SIZE; y++) {
        for (int x = 0; x < m_gridCountX; x++) {

            //if (x == 3 && y == 3) continue;

            // Skip fat items
            if (itemCellSize > 1) {
                if (x + 1 >= m_gridCountX)     continue; // range check
                if (m_occupiedSlots[x + 1][y]) continue; // cell check
            }
            if (itemCellSize > 2) {
                if (x + 2 >= m_gridCountX)     continue; // range check
                if (m_occupiedSlots[x + 2][y]) continue; // cell check
            }
            // Free slot found
            if (!m_occupiedSlots[x][y]) {
                return glm::ivec2(x, y);
            }
        }
    }
    // Now scan for a rotated slot
    for (int y = 0; y < MAX_INVENTORY_Y_SIZE; y++) {
        for (int x = 0; x < m_gridCountX; x++) {
            // TODO
        }
    }

    // No space available
    return glm::ivec2(-1, -1);
}

void Inventory::PrintGridOccupiedStateToConsole() { 
    std::cout << " ";
    for (int y = 0; y < MAX_INVENTORY_Y_SIZE; y++) {
        for (int x = 0; x < m_gridCountX; x++) {
            std::cout << "[";
            if (m_occupiedSlots[x][y]) {
                std::cout << "X";
            }
            else {
                std::cout << " ";
            }
            std::cout << "]";
        }
        std::cout << "\n ";
    }
}

//InventoryItem* Inventory::GetSelectedItem() {
//    if (m_selectedItem)
//}