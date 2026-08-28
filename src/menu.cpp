#include "menu.hpp"
#include "raylib.h"
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>

struct HelpSection {
    std::string title;
    std::string body;
};

struct SaveSlotInfo {
    int slotNumber;
    std::string filename;
    bool exists;
    std::string details;
};

static std::vector<std::string> WrapText(Font font, const std::string& text, float maxWidth, float fontSize, float spacing) {
    std::vector<std::string> lines;
    std::string word;
    std::string line;
    for (size_t i = 0; i <= text.size(); ++i) {
        char ch = (i < text.size()) ? text[i] : ' ';
        if (ch == ' ' || ch == '\n') {
            if (!word.empty()) {
                std::string candidate = line.empty() ? word : line + " " + word;
                Vector2 size = MeasureTextEx(font, candidate.c_str(), fontSize, spacing);
                if (size.x > maxWidth && !line.empty()) {
                    lines.push_back(line);
                    line = word;
                } else {
                    line = candidate;
                }
                word.clear();
            }
            if (ch == '\n' && !line.empty()) {
                lines.push_back(line);
                line.clear();
            }
        } else {
            word += ch;
        }
    }
    if (!line.empty()) lines.push_back(line);
    return lines;
}

static void DrawCenteredText(Font font, const std::string& text, float centerX, float y, float fontSize, float spacing, Color color) {
    Vector2 size = MeasureTextEx(font, text.c_str(), fontSize, spacing);
    DrawTextEx(font, text.c_str(), {centerX - size.x / 2.0f, y}, fontSize, spacing, color);
}

static std::vector<SaveSlotInfo> getSaveSlots() {
    std::vector<SaveSlotInfo> slots;
    for (int i = 1; i <= 3; ++i) {
        std::string fname = "save_slot_" + std::to_string(i) + ".txt";
        std::ifstream inFile(fname);
        SaveSlotInfo info;
        info.slotNumber = i;
        info.filename = fname;
        if (inFile.is_open()) {
            info.exists = true;
            std::string teamHeader, turnLine;
            std::getline(inFile, teamHeader);
            std::getline(inFile, turnLine);
            info.details = "Slot " + std::to_string(i) + " - " + (turnLine.empty() ? "Saved Game" : turnLine);
            inFile.close();
        } else {
            info.exists = false;
            info.details = "Empty Slot " + std::to_string(i);
        }
        slots.push_back(info);
    }
    std::sort(slots.begin(), slots.end(), [](const SaveSlotInfo& a, const SaveSlotInfo& b) {
        if (!a.exists || !b.exists) return a.exists > b.exists;
        auto tA = std::filesystem::last_write_time(a.filename);
        auto tB = std::filesystem::last_write_time(b.filename);
        return tA > tB;
    });
    return slots;
}

MenuResult Menu::show() {
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Unmatched - Main Menu");
    SetTargetFPS(60);

    Texture2D bg = LoadTexture("assets/menu_bg.png");
    if (bg.id != 0) SetTextureFilter(bg, TEXTURE_FILTER_BILINEAR);

    Font titleFont = LoadFontEx("assets/fonts/Cinzel-Black.ttf", 110, nullptr, 0);
    Font semiFont = LoadFontEx("assets/fonts/Cinzel-SemiBold.ttf", 56, nullptr, 0);
    Font regularFont = LoadFontEx("assets/fonts/Cinzel-Regular.ttf", 56, nullptr, 0);

    if (titleFont.texture.id != 0) SetTextureFilter(titleFont.texture, TEXTURE_FILTER_BILINEAR);
    if (semiFont.texture.id != 0) SetTextureFilter(semiFont.texture, TEXTURE_FILTER_BILINEAR);
    if (regularFont.texture.id != 0) SetTextureFilter(regularFont.texture, TEXTURE_FILTER_BILINEAR);

    Rectangle startBtn = { screenWidth / 2.0f - 210, 420, 420, 64 };
    Rectangle loadBtn   = { screenWidth / 2.0f - 210, 505, 420, 56 };
    Rectangle helpBtn   = { screenWidth / 2.0f - 210, 580, 420, 56 };
    Rectangle exitBtn   = { screenWidth / 2.0f - 210, 655, 420, 56 };

    Color gold = {212, 175, 55, 255};
    Color goldLight = {238, 205, 92, 255};
    Color glass = {20, 24, 38, 190};
    Color glassHover = {32, 38, 58, 220};
    Color redBtn = {135, 12, 18, 255};
    Color redHover = {165, 20, 28, 255};
    Color textColor = {230, 226, 218, 255};
    Color dimColor = {165, 160, 150, 255};

    std::vector<HelpSection> helpSections = {
        {"MENU", "START begins a new match. You will first enter the Age Assessment screen and then choose your hero."},
        {"LOAD GAME", "LOAD GAME resumes the most recently saved match from one of the available save slots."},
        {"AGE ASSESSMENT", "Enter the ages of both players. The younger player chooses a hero first. If both players have the same age, the game randomly decides which player chooses first."},
        {"HERO SELECTION", "The first player chooses one hero. The second player then chooses a different hero from the remaining fighters. Available heroes are Dracula, Invisible Man, and Sherlock Holmes."},
        {"THE GAME BOARD", "After hero selection, both players begin on opposite sides of the board. Your hero and sidekicks occupy spaces on the map and move through connected zones."},
        {"YOUR TURN", "During your turn, use your available actions to move, attack, or play card effects. Think carefully before spending your actions because your opponent will act next."},
        {"CARDS", "Cards can be used for different purposes depending on the situation. Attack cards are used during combat, defense cards protect your characters, and versatile cards may provide movement or special effects."},
        {"MOVEMENT", "Use movement cards to move your hero and sidekicks across the board. Your characters can only move through connected spaces according to the board rules."},
        {"COMBAT", "Choose an attacking character and a valid target. Play an attack card, then the defending player chooses a defense card when available. The attack and defense values determine the result of combat."},
        {"SPECIAL EFFECTS", "Some cards have special effects. These effects can change movement, damage, defense, positioning, or other parts of the game. Read the card carefully before playing it."},
        {"SIDEKICKS", "Sidekicks support their heroes during the battle. Dracula is supported by the Sisters, while Sherlock Holmes is supported by Watson. Sidekicks can move, attack, defend, and occupy spaces on the board."},
        {"INVISIBLE MAN", "The Invisible Man uses the fog to his advantage. Fog tokens can affect the spaces around him and help him move through the battlefield while remaining difficult to track."},
        {"VICTORY", "Reduce the opposing hero's health to 0 to win the game. Protect your own hero while using your cards and abilities to defeat your opponent."}
    };

    bool showHelp = false;
    bool helpJustOpened = false;
    float helpScroll = 0.0f;

    bool showLoadSelection = false;
    std::vector<SaveSlotInfo> slots = getSaveSlots();

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        if (showLoadSelection) {
            float modalW = 480.0f;
            float modalH = 380.0f;
            float modalX = (screenWidth - modalW) / 2.0f;
            float modalY = (screenHeight - modalH) / 2.0f;
            Rectangle modalRect = {modalX, modalY, modalW, modalH};

            BeginDrawing();
            ClearBackground(BLACK);
            if (bg.id != 0) {
                DrawTexturePro(bg, {0,0,(float)bg.width,(float)bg.height},
                               {0,0,(float)screenWidth,(float)screenHeight}, {0,0}, 0, WHITE);
            }
            DrawRectangle(0, 0, screenWidth, screenHeight, {0,0,0,180});

            DrawRectangleRounded(modalRect, 0.05f, 20, {14,12,20,240});
            DrawRectangleRoundedLines(modalRect, 0.05f, 20, 2.0f, gold);
            DrawRectangleRoundedLines({modalX+7, modalY+7, modalW-14, modalH-14}, 0.05f, 20, 1.0f, {212,175,55,65});

            DrawCenteredText(titleFont, "LOAD GAME", modalX + modalW/2.0f, modalY + 20, 28, 1.0f, goldLight);
            DrawCenteredText(regularFont, "Select a saved game to continue", modalX + modalW/2.0f, modalY + 62, 14, 0.6f, dimColor);
            DrawLine((int)(modalX+40), (int)(modalY+85), (int)(modalX+modalW-40), (int)(modalY+85), {212,175,55,80});

            float startY = modalY + 105;
            float slotHeight = 42.0f;
            float slotGap = 8.0f;
            for (size_t i = 0; i < slots.size(); ++i) {
                const auto& slot = slots[i];
                Rectangle slotBtn = {modalX + 40, startY + i * (slotHeight + slotGap), modalW - 80, slotHeight};
                bool hover = CheckCollisionPointRec(mouse, slotBtn);

                Color bgColor = hover ? glassHover : glass;
                if (!slot.exists) bgColor = {40,35,45,180};

                DrawRectangleRounded(slotBtn, 0.15f, 12, bgColor);
                DrawRectangleRoundedLines(slotBtn, 0.15f, 12, 1.2f, slot.exists ? gold : dimColor);

                std::string label = slot.details;
                if (!slot.exists) label = "[" + label + "]";
                DrawTextEx(semiFont, label.c_str(), {slotBtn.x + 15, slotBtn.y + 11}, 16, 0.5f,
                           slot.exists ? textColor : dimColor);

                if (slot.exists && hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    UnloadFont(titleFont);
                    UnloadFont(semiFont);
                    UnloadFont(regularFont);
                    UnloadTexture(bg);
                    CloseWindow();
                    return {MenuResult::Load, slot.slotNumber};
                }
            }

            Rectangle backBtn = {modalX + (modalW - 160)/2, modalY + modalH - 50, 160, 36};
            bool hoverBack = CheckCollisionPointRec(mouse, backBtn);
            DrawRectangleRounded(backBtn, 0.2f, 12, hoverBack ? redHover : redBtn);
            DrawRectangleRoundedLines(backBtn, 0.2f, 12, 1.2f, {255,255,255,35});
            DrawCenteredText(semiFont, "BACK", backBtn.x + 80, backBtn.y + 8, 16, 0.6f, WHITE);

            if (hoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                showLoadSelection = false;
            }

            EndDrawing();
            continue;
        }

        bool hoverStart = !showHelp && CheckCollisionPointRec(mouse, startBtn);
        bool hoverLoad  = !showHelp && CheckCollisionPointRec(mouse, loadBtn);
        bool hoverHelp  = !showHelp && CheckCollisionPointRec(mouse, helpBtn);
        bool hoverExit  = !showHelp && CheckCollisionPointRec(mouse, exitBtn);

        if (hoverStart && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            UnloadFont(titleFont);
            UnloadFont(semiFont);
            UnloadFont(regularFont);
            UnloadTexture(bg);
            CloseWindow();
            return {MenuResult::Start, 0};
        }

        if (hoverLoad && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showLoadSelection = true;
            slots = getSaveSlots();
        }

        if (hoverExit && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            UnloadFont(titleFont);
            UnloadFont(semiFont);
            UnloadFont(regularFont);
            UnloadTexture(bg);
            CloseWindow();
            return {MenuResult::Exit, 0};
        }

        if (hoverHelp && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showHelp = true;
            helpJustOpened = true;
            helpScroll = 0.0f;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        if (bg.id != 0) {
            DrawTexturePro(bg, {0,0,(float)bg.width,(float)bg.height},
                           {0,0,(float)screenWidth,(float)screenHeight}, {0,0}, 0, WHITE);
        }
        DrawRectangle(0, 0, screenWidth, screenHeight, {0,0,0,140});

        if (!showHelp) {
            DrawCenteredText(regularFont, "THE ETERNAL BATTLE OF SHADOWS", screenWidth/2.0f, 250, 20, 1.2f, {180,140,70,255});
            DrawCenteredText(titleFont, "UNMATCHED", screenWidth/2.0f, 285, 76, 1.0f, gold);

            DrawRectangleRounded(startBtn, 0.22f, 20, hoverStart ? redHover : redBtn);
            DrawRectangleRoundedLines(startBtn, 0.22f, 20, 1.2f, {255,255,255,35});
            DrawCenteredText(semiFont, "START", startBtn.x + startBtn.width/2.0f, startBtn.y + 17, 22, 1.0f, WHITE);

            DrawRectangleRounded(loadBtn, 0.22f, 20, hoverLoad ? glassHover : glass);
            DrawRectangleRoundedLines(loadBtn, 0.22f, 20, 1.2f, {255,255,255,35});
            DrawCenteredText(regularFont, "LOAD GAME", loadBtn.x + loadBtn.width/2.0f + 12, loadBtn.y + 16, 19, 0.8f, {230,230,235,255});

            DrawRectangleRounded(helpBtn, 0.22f, 20, hoverHelp ? glassHover : glass);
            DrawRectangleRoundedLines(helpBtn, 0.22f, 20, 1.2f, {255,255,255,35});
            DrawCenteredText(regularFont, "HELP", helpBtn.x + helpBtn.width/2.0f, helpBtn.y + 16, 19, 0.8f, {230,230,235,255});

            DrawRectangleRounded(exitBtn, 0.22f, 20, hoverExit ? glassHover : glass);
            DrawRectangleRoundedLines(exitBtn, 0.22f, 20, 1.2f, {255,255,255,35});
            DrawCenteredText(regularFont, "EXIT", exitBtn.x + exitBtn.width/2.0f, exitBtn.y + 16, 19, 0.8f, {230,230,235,255});
        }

        if (showHelp) {
            DrawRectangle(0, 0, screenWidth, screenHeight, {0,0,0,200});
            Rectangle panel = {screenWidth / 2.0f - 480, 28, 960, 744};
            Rectangle shadow = {panel.x + 10, panel.y + 12, panel.width, panel.height};
            DrawRectangleRounded(shadow, 0.05f, 24, {0,0,0,130});
            DrawRectangleRounded(panel, 0.05f, 24, {14,12,20,252});
            DrawRectangleRoundedLines(panel, 0.05f, 24, 2.0f, gold);
            DrawRectangleRoundedLines({panel.x+7, panel.y+7, panel.width-14, panel.height-14}, 0.05f, 24, 1.0f, {212,175,55,65});

            Rectangle closeBtn = {panel.x + panel.width - 55, panel.y + 14, 40, 40};
            bool hoverClose = CheckCollisionPointRec(mouse, closeBtn);
            DrawRectangleRounded(closeBtn, 0.3f, 12, hoverClose ? glassHover : glass);
            DrawRectangleRoundedLines(closeBtn, 0.3f, 12, 1.2f, {255,255,255,45});
            Color xColor = hoverClose ? Color{235,235,235,255} : Color{190,185,178,255};
            DrawLineEx({closeBtn.x+11, closeBtn.y+11}, {closeBtn.x+closeBtn.width-11, closeBtn.y+closeBtn.height-11}, 2.5f, xColor);
            DrawLineEx({closeBtn.x+closeBtn.width-11, closeBtn.y+11}, {closeBtn.x+11, closeBtn.y+closeBtn.height-11}, 2.5f, xColor);

            DrawCenteredText(titleFont, "HOW TO PLAY", panel.x + panel.width/2.0f, panel.y + 34, 38, 1.0f, goldLight);
            DrawCenteredText(regularFont, "UNMATCHED: GOTHIC SHADOWS", panel.x + panel.width/2.0f, panel.y + 87, 14, 1.2f, {180,140,70,255});
            DrawLine((int)(panel.x+65), (int)(panel.y+120), (int)(panel.x+panel.width-65), (int)(panel.y+120), {212,175,55,90});

            Rectangle backBtn = {panel.x + panel.width/2.0f - 130, panel.y + panel.height - 65, 260, 48};
            Rectangle textArea = {panel.x + 65, panel.y + 140, panel.width - 130, panel.height - 220};
            float contentWidth = textArea.width - 15.0f;
            float sectionTitleSize = 19.0f;
            float bodySize = 17.0f;
            float bodySpacing = 0.4f;
            float lineHeight = 27.0f;
            float sectionGap = 18.0f;

            struct RenderLine { std::string text; bool title; };
            std::vector<RenderLine> renderLines;
            for (const auto& section : helpSections) {
                renderLines.push_back({section.title, true});
                std::vector<std::string> wrapped = WrapText(regularFont, section.body, contentWidth, bodySize, bodySpacing);
                for (const auto& line : wrapped) renderLines.push_back({line, false});
                renderLines.push_back({"", false});
            }

            float totalHeight = 0.0f;
            for (const auto& line : renderLines) {
                if (line.title) totalHeight += 30.0f;
                else totalHeight += lineHeight;
            }
            totalHeight += sectionGap;

            float maxScroll = totalHeight - textArea.height;
            if (maxScroll < 0.0f) maxScroll = 0.0f;

            if (CheckCollisionPointRec(mouse, textArea)) {
                float wheel = GetMouseWheelMove();
                if (wheel != 0.0f) helpScroll -= wheel * 35.0f;
            }
            if (helpScroll < 0.0f) helpScroll = 0.0f;
            if (helpScroll > maxScroll) helpScroll = maxScroll;

            BeginScissorMode((int)textArea.x, (int)textArea.y, (int)textArea.width, (int)textArea.height);
            float currentY = textArea.y - helpScroll;
            for (const auto& line : renderLines) {
                if (line.title) {
                    if (currentY + 25 >= textArea.y && currentY <= textArea.y + textArea.height) {
                        DrawTextEx(semiFont, line.text.c_str(), {textArea.x, currentY}, sectionTitleSize, 0.8f, gold);
                    }
                    currentY += 30.0f;
                } else {
                    if (!line.text.empty()) {
                        if (currentY + lineHeight >= textArea.y && currentY <= textArea.y + textArea.height) {
                            DrawTextEx(regularFont, line.text.c_str(), {textArea.x, currentY}, bodySize, bodySpacing, textColor);
                        }
                        currentY += lineHeight;
                    } else {
                        currentY += sectionGap;
                    }
                }
            }
            EndScissorMode();

            if (maxScroll > 0.0f) {
                float scrollbarWidth = 5.0f;
                Rectangle track = {textArea.x + textArea.width - scrollbarWidth, textArea.y, scrollbarWidth, textArea.height};
                DrawRectangleRounded(track, 0.5f, 4, {70,70,75,120});
                float thumbHeight = textArea.height * (textArea.height / totalHeight);
                if (thumbHeight < 35.0f) thumbHeight = 35.0f;
                float thumbY = textArea.y + (textArea.height - thumbHeight) * (helpScroll / maxScroll);
                Rectangle thumb = {track.x, thumbY, scrollbarWidth, thumbHeight};
                DrawRectangleRounded(thumb, 0.5f, 4, gold);
            }

            bool hoverBackHelp = CheckCollisionPointRec(mouse, backBtn);
            DrawRectangleRounded(backBtn, 0.22f, 20, hoverBackHelp ? redHover : redBtn);
            DrawRectangleRoundedLines(backBtn, 0.22f, 20, 1.2f, {255,255,255,35});
            DrawCenteredText(semiFont, "BACK", backBtn.x + backBtn.width/2.0f, backBtn.y + 13, 18, 1.0f, WHITE);

            bool closePressed = hoverClose && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
            bool backPressed = hoverBackHelp && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
            bool escapePressed = IsKeyPressed(KEY_ESCAPE);

            if (!helpJustOpened && (closePressed || backPressed || escapePressed)) {
                showHelp = false;
                helpScroll = 0.0f;
            }
        }

        helpJustOpened = false;
        EndDrawing();
    }

    UnloadFont(titleFont);
    UnloadFont(semiFont);
    UnloadFont(regularFont);
    UnloadTexture(bg);
    CloseWindow();

    return {MenuResult::Exit, 0};
}