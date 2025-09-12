#include "Inventory.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Bible.h"
#include "Input/Input.h"
#include "UI/TextBlitter.h"
#include "UI/UiBackend.h"

void Inventory::SubmitRenderItems() {
    Texture* bgTexture = AssetManager::GetTextureByName("inventory_bg");
    Texture* squareSize1Texture = AssetManager::GetTextureByName("InvSquare_Size1");
    Texture* squareSize2Texture = AssetManager::GetTextureByName("InvSquare_Size2");
    Texture* squareSize3Texture = AssetManager::GetTextureByName("InvSquare_Size3");
    Texture* squareSize1SelectedTexture = AssetManager::GetTextureByName("InvSquare_Size1");
    //Texture* squareSize2SelectedTexture = AssetManager::GetTextureByName("InvSquare_Size2");
    //Texture* squareSize3SelectedTexture = AssetManager::GetTextureByName("InvSquare_Size3");

    if (!bgTexture) return;
    if (!squareSize1Texture) return;
    if (!squareSize2Texture) return;
    if (!squareSize3Texture) return;
    if (!squareSize1SelectedTexture) return;

    int cellWidth = squareSize1Texture->GetWidth(0);
    int cellHeight = squareSize1Texture->GetHeight(0);

    glm::ivec2 origin = glm::ivec2(100, 100);
    glm::ivec2 size = glm::ivec2(474, 855);


    BlitTextureInfo blitTextureInfo;
    blitTextureInfo.textureName = "inventory_lazybg";
    blitTextureInfo.location = origin;
    blitTextureInfo.alignment = Alignment::TOP_LEFT;
    blitTextureInfo.textureFilter = TextureFilter::LINEAR;
    UIBackEnd::BlitTexture(blitTextureInfo);

    // Squares
    glm::ivec2 squaresOrigin = origin + glm::ivec2(49, 49);

    int itemCountX = 4;
    int itemCountY = 4;

    for (int x = 0; x < itemCountX; x++) {
        for (int y = 0; y < itemCountY; y++) {
            glm::ivec2 location;
            location.x = squaresOrigin.x + (squareSize1Texture->GetWidth(0) * x);
            location.y = squaresOrigin.y + (squareSize1Texture->GetHeight(0) * y);
            BlitTextureInfo blitTextureInfo;
            blitTextureInfo.textureName = squareSize1Texture->GetFileName();
            blitTextureInfo.location = location;
            blitTextureInfo.alignment = Alignment::TOP_LEFT;
            blitTextureInfo.textureFilter = TextureFilter::LINEAR;
            UIBackEnd::BlitTexture(blitTextureInfo);


          // if (m_selectedCellX == x && m_selectedCellY == y) {
          //     blitTextureInfo.textureName = squareSize1SelectedTexture->GetFileName();
          //     UIBackEnd::BlitTexture(blitTextureInfo);
          // }

        }
    }

    // Icons
    for (InventoryItem& item : m_items) {
        // Lookup texture name based on item name + prefix
        std::string textureName = "InvItem_" + item.m_name;
        Texture* texture = AssetManager::GetTextureByName(textureName);
        if (!texture) {
            std::cout << "Could not render inventory icon for '" << item.m_name << "' coz ;" << textureName << "' was not found\n";
            continue;
        }

        InventoryItemInfo* itemInfo = Bible::GetInventoryItemInfoByName(item.m_name);
        if (!itemInfo) continue;

        glm::ivec2 itemLocation;
        itemLocation.x = squaresOrigin.x + (cellWidth * item.m_gridLocation.x);
        itemLocation.y = squaresOrigin.y + (cellHeight * item.m_gridLocation.y);

        // Render bigger cell square backgrounds if needed
        if (itemInfo->m_cellSize == 2) {
            BlitTextureInfo blitTextureInfo;
            blitTextureInfo.textureName = squareSize2Texture->GetFileName();
            blitTextureInfo.location = itemLocation;
            blitTextureInfo.alignment = Alignment::TOP_LEFT;
            blitTextureInfo.textureFilter = TextureFilter::LINEAR;
            UIBackEnd::BlitTexture(blitTextureInfo);

            // TODO: account for rotation
        }
        if (itemInfo->m_cellSize == 3) {
            BlitTextureInfo blitTextureInfo;
            blitTextureInfo.textureName = squareSize3Texture->GetFileName();
            blitTextureInfo.location = itemLocation;
            blitTextureInfo.alignment = Alignment::TOP_LEFT;
            blitTextureInfo.textureFilter = TextureFilter::LINEAR;
            UIBackEnd::BlitTexture(blitTextureInfo);

            // TODO: account for rotation
        }

        // Render icon
        BlitTextureInfo blitTextureInfo;
        blitTextureInfo.textureName = textureName;
        blitTextureInfo.location = itemLocation;
        blitTextureInfo.alignment = Alignment::TOP_LEFT;
        blitTextureInfo.textureFilter = TextureFilter::LINEAR;
        UIBackEnd::BlitTexture(blitTextureInfo);
    }


    //// Background
    //int bgWidth = bgTexture->GetWidth(0);
    //int bgHeight = bgTexture->GetHeight(0);
    //int bgCountX = size.x / bgWidth;
    //int bgCountY = size.y / bgHeight;
    //int inventoryMinX = origin.x;
    //int inventoryMinY = origin.y;
    //int inventoryMaxX = origin.x + size.x;
    //int inventoryMaxY = origin.y + size.y;
    //
    //for (int x = 0; x < bgCountX + 1; x++) {
    //    for (int y = 0; y < bgCountY + 1; y++) {
    //        BlitTextureInfo blitTextureInfo;
    //        blitTextureInfo.textureName = "inventory_bg";
    //        blitTextureInfo.location = origin + glm::ivec2(x * bgWidth, y * bgHeight);
    //        blitTextureInfo.alignment = Alignment::TOP_LEFT;
    //        blitTextureInfo.textureFilter = TextureFilter::LINEAR;
    //        blitTextureInfo.clipMinX = inventoryMinX;
    //        blitTextureInfo.clipMinY = inventoryMaxY;
    //        blitTextureInfo.clipMaxX = inventoryMinX;
    //        blitTextureInfo.clipMaxY = inventoryMaxY;
    //        UIBackEnd::BlitTexture(blitTextureInfo);
    //    }
    //}

    // Text
    int marginX = squaresOrigin.x;
    int headingPadding = 30;
    int theLineY = squaresOrigin.y + (cellHeight * itemCountY) + headingPadding;
    int headingY = theLineY + 32;
    int descriptionY = headingY + 50;



    // The line
    BlitTextureInfo lineInfo;
    lineInfo.textureName = "inv_the_line";
    lineInfo.location = { marginX, theLineY };
    lineInfo.alignment = Alignment::TOP_LEFT;
    lineInfo.textureFilter = TextureFilter::LINEAR;
    UIBackEnd::BlitTexture(lineInfo);


    static bool test = true;
    if (Input::KeyPressed(HELL_KEY_9)) {
        test = !test;
    }

    std::string name = "GLOCK 22";
    std::string description = R"(Australian police issue. Matte and boxy, a cold
little companion. It does the paperwork duty 
without drama. Dependable at short range, 
underwhelming at a distance. A proper piece 
of shit.)";

    if (!test) {
        name = "RED DOT SIGHT";
        description = R"(A little window with a floating promise. Turns
aim into point, and point into a bloody mess.)";
    }


    UIBackEnd::BlitText("[COL=0.839,0.784,0.635]" + name, "BebasNeue", marginX, headingY, Alignment::TOP_LEFT, 1.0f, TextureFilter::LINEAR);
    UIBackEnd::BlitText("[COL=0.839,0.784,0.635]" + description, "RobotoCondensed", marginX, descriptionY, Alignment::TOP_LEFT, 1.0f, TextureFilter::LINEAR);



    glm::ivec2 descriptonSize = TextBlitter::GetBlitTextSize(description, "RobotoCondensed", 1.0f);

    int thePlaceholderY = descriptionY + descriptonSize.y;

    // The placeholder
    BlitTextureInfo placeholderInfo;
    placeholderInfo.textureName = "inv_the_line_white";
    placeholderInfo.location = { marginX, descriptionY };
    placeholderInfo.alignment = Alignment::TOP_LEFT;
    placeholderInfo.textureFilter = TextureFilter::LINEAR;
    placeholderInfo.size = { descriptonSize.x, 2 };
   // UIBackEnd::BlitTexture(placeholderInfo);


    placeholderInfo.textureName = "inv_the_line_white";
    placeholderInfo.location = { marginX, thePlaceholderY };
    placeholderInfo.alignment = Alignment::TOP_LEFT;
    placeholderInfo.textureFilter = TextureFilter::LINEAR;
  //  UIBackEnd::BlitTexture(placeholderInfo);
    placeholderInfo.size = { descriptonSize.x, 2 };



    placeholderInfo.textureName = "White";
    placeholderInfo.location = { marginX + descriptonSize.x, descriptionY };
    placeholderInfo.alignment = Alignment::TOP_LEFT;
    placeholderInfo.textureFilter = TextureFilter::LINEAR;
    placeholderInfo.size = { 2, descriptonSize.y };
   // UIBackEnd::BlitTexture(placeholderInfo);

    placeholderInfo.textureName = "White";
    placeholderInfo.location = { marginX, descriptionY };
    placeholderInfo.alignment = Alignment::TOP_LEFT;
    placeholderInfo.textureFilter = TextureFilter::LINEAR;
    placeholderInfo.size = { 2, descriptonSize.y };
    //UIBackEnd::BlitTexture(placeholderInfo);


    int buttonPaddingY = 35;
    int buttonMarginX = marginX; // maybe you wanna indent this?
    int buttonOriginY = descriptionY + descriptonSize.y + buttonPaddingY;
    int buttonLineHeight = 31;


    RenderButton(glm::ivec2(buttonMarginX, buttonOriginY), "E", "Equip");
    buttonOriginY += buttonLineHeight;

    RenderButton(glm::ivec2(buttonMarginX, buttonOriginY), "C", "Combine");
    buttonOriginY += buttonLineHeight;

    RenderButton(glm::ivec2(buttonMarginX, buttonOriginY), "F", "Examine");
    buttonOriginY += buttonLineHeight;

    RenderButton(glm::ivec2(buttonMarginX, buttonOriginY), "M", "Move");
    buttonOriginY += buttonLineHeight;

    RenderButton(glm::ivec2(buttonMarginX, buttonOriginY), "G", "Discard");
    buttonOriginY += buttonLineHeight;


    if (false) {
        UIBackEnd::BlitTexture("White", origin, Alignment::TOP_LEFT, ORANGE, glm::ivec2(1, size.y), TextureFilter::LINEAR);
        UIBackEnd::BlitTexture("White", origin, Alignment::TOP_LEFT, ORANGE, glm::ivec2(size.x, 1), TextureFilter::LINEAR);
        UIBackEnd::BlitTexture("White", origin + size, Alignment::BOTTOM_RIGHT, ORANGE, glm::ivec2(1, size.y), TextureFilter::LINEAR);
        UIBackEnd::BlitTexture("White", origin + size, Alignment::BOTTOM_RIGHT, ORANGE, glm::ivec2(size.x, 1), TextureFilter::LINEAR);
    }
}


void Inventory::RenderButton(glm::ivec2 location, const std::string& letter, const std::string& description) {
    Texture* buttonTexture = AssetManager::GetTextureByName("inventory_green_button");
    if (!buttonTexture) return;

    int buttonCenterX = location.x + (buttonTexture->GetWidth(0) * 0.5f);
    int descriptionLeftY = buttonCenterX + 16;

    UIBackEnd::BlitTexture("inventory_green_button", glm::ivec2(buttonCenterX, location.y), Alignment::CENTERED, WHITE, glm::ivec2(-1, -1), TextureFilter::LINEAR);
    UIBackEnd::BlitText("[COL=0.839,0.784,0.635]" + letter, "RobotoCondensed", buttonCenterX, location.y + 1, Alignment::CENTERED, 0.75f, TextureFilter::LINEAR);
    UIBackEnd::BlitText("[COL=0.839,0.784,0.635]" + description, "RobotoCondensed", descriptionLeftY, location.y + 1, Alignment::CENTERED_VERTICAL, 1.0f, TextureFilter::LINEAR);

}
