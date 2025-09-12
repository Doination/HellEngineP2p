#pragma once
#include "Inventory/Inventory.h"
#include <string>
#include <vector>

struct InventoryItemInfo {
    std::string m_name = UNDEFINED_STRING;
    std::string m_itemHeading = UNDEFINED_STRING;
    std::string m_description = UNDEFINED_STRING;
    InventoryItemType m_itemType = InventoryItemType::UESLESS;
    int m_cellSize = 1;
    bool m_combineable = false;
    bool m_discardable = true;
    bool m_equipable = true;
};

namespace Bible {
    void Init();

    InventoryItemInfo* GetInventoryItemInfoByName(const std::string& name);
}