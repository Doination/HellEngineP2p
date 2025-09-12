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
        glock.m_name = "Glock"; // make a helper CreateInventoryItem() function to fill this out based on unordered map key 
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
        goldenGlock.m_name = "GoldenGlock"; // make a helper CreateInventoryItem() function to fill this out based on unordered map key
        goldenGlock.m_cellSize = 1;
        goldenGlock.m_combineable = true;
        goldenGlock.m_discardable = true;
        goldenGlock.m_equipable = true;
        goldenGlock.m_itemHeading = "GOLDEN GLOCK 22";
        goldenGlock.m_description = R"(Shaken naked, not stirred. Pierce Brosnan's 
wet dream, wedding gift dipped in drip and 
glitter. Natalia gonna be in the good books 
for this one.)";

        InventoryItemInfo& tokarev = m_inventoryItems["Tokarev"];
        tokarev.m_name = "Tokarev"; // make a helper CreateInventoryItem() function to fill this out based on unordered map key 
        tokarev.m_cellSize = 1;
        tokarev.m_combineable = true;
        tokarev.m_discardable = true;
        tokarev.m_equipable = true;
        tokarev.m_itemHeading = "TOKAREV";
        tokarev.m_description = R"(Soviet semi-automatic pistol developed in the
1920s. This baby runs hot and she got no frills, 
just prints pretty little holes. Straight out 
of Ourumov's briefcase.)";

        InventoryItemInfo& knife = m_inventoryItems["Knife"];
        knife.m_name = "Knife"; // make a helper CreateInventoryItem() function to fill this out based on unordered map key
        knife.m_cellSize = 1;
        knife.m_combineable = true;
        knife.m_discardable = true;
        knife.m_equipable = true;
        knife.m_itemHeading = "KNIFE";
        knife.m_description = R"(From fish to pharynx, this rusty little dagger 
takes the same short and messy path. Might
wanna bring a cloth.)";

        InventoryItemInfo& remington870 = m_inventoryItems["Remington870"];
        remington870.m_name = "Remington870"; // make a helper CreateInventoryItem() function to fill this out based on unordered map key
        remington870.m_cellSize = 3;
        remington870.m_combineable = false;
        remington870.m_discardable = true;
        remington870.m_equipable = true;
        remington870.m_itemHeading = "REMINGTON 870";
        remington870.m_description = R"(American pump-action built like a fence post. 
This twelve-gauge thunder will clear every 
damn room and barn in time for Judgment 
Day supper.)";


        InventoryItemInfo& spas = m_inventoryItems["SPAS"];
        spas.m_name = "SPAS"; // make a helper CreateInventoryItem() function to fill this out based on unordered map key
        spas.m_cellSize = 3;
        spas.m_combineable = false;
        spas.m_discardable = true;
        spas.m_equipable = true;
        spas.m_itemHeading = "SPAS";
        spas.m_description = R"(Dual modal, Italian menace. If killing is a sport, 
then Franchi's Special Purpose Automatic 
Shotgun will put you on the podium with the 
cadence of a fucking riot.
)";


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

    int GetInventoryItemSizeByName(const std::string& name) {
        InventoryItemInfo* itemInfo = GetInventoryItemInfoByName(name);
        if (!itemInfo) return 0;
        else return itemInfo->m_cellSize;
    }
}