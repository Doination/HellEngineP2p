#pragma once
#include "Inventory/Inventory.h"
#include <string>
#include <vector>

namespace Bible {
    void Init();

    int GetInventoryItemSizeByName(const std::string& name);
    InventoryItemInfo* GetInventoryItemInfoByName(const std::string& name);
}