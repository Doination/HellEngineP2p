#include "Bible.h"
#include <iostream>
#include <unordered_map>

namespace Bible {
    std::unordered_map<std::string, InventoryItemInfo> m_inventoryItems;

    void InitInventoryInfo();
    void InitWeaponInfo();

    void Init() {
        InitInventoryInfo();
        InitWeaponInfo();
        std::cout << "The Bible has been read\n";
    }

    void InitInventoryInfo() {
        m_inventoryItems.clear();

        InventoryItemInfo& glock = m_inventoryItems["Glock"];
        glock.m_cellSize = 1;
        glock.m_combineable = true;
        glock.m_discardable = true;
        glock.m_equipable = true;
        glock.m_itemHeading = "GLOCK 22";
        glock.m_description = R"(Australian police issue. Matte and boxy, a cold
little companion. It does the paperwork duty 
without drama. Dependable at short range, 
underwhelming at a distance. A proper piece 
of shit.)";

        InventoryItemInfo& goldenGlock = m_inventoryItems["GoldenGlock"];
        goldenGlock.m_cellSize = 1;
        goldenGlock.m_combineable = true;
        goldenGlock.m_discardable = true;
        goldenGlock.m_equipable = true;
        goldenGlock.m_itemHeading = "GOLDEN GLOCK 22";
        goldenGlock.m_description = R"(Shaken naked, not stirred. Pierce Brosnan's 
wet dream, wedding gift dipped in drip and 
glitter. Natalia gonna be in the good books 
for this one.)";

        InventoryItemInfo& knife = m_inventoryItems["Knife"];
        knife.m_cellSize = 1;
        knife.m_combineable = true;
        knife.m_discardable = true;
        knife.m_equipable = true;
        knife.m_itemHeading = "KNIFE";
        knife.m_description = R"(From fish to pharynx, this rusty little dagger 
takes the same short and messy path. Might
wanna bring a cloth.)";

        InventoryItemInfo& remington870 = m_inventoryItems["Remington870"];
        remington870.m_cellSize = 3;
        remington870.m_combineable = false;
        remington870.m_discardable = true;
        remington870.m_equipable = true;
        remington870.m_itemHeading = "REMINGTON 870";
        remington870.m_description = R"(American pump-action built like a fence post. 
This twelve-gauge thunder will clear every 
damn room and barn in time for Judgment 
Day supper.)";



    }

    void InitWeaponInfo() {
        // TODO: Move everything from WeaponManager in here
    }

    InventoryItemInfo* GetInventoryItemInfoByName(const std::string& name) {
        if (m_inventoryItems.find(name) != m_inventoryItems.end()) {
            return &m_inventoryItems[name];
        }
        else {
            std::cout << "GetInventoryItemInfoByName::(name) failed because '" << name << "' was not found in m_inventoryItems\n";
            return nullptr;
        }
    }
}