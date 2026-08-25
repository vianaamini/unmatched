#include "menu.hpp"
#include "raylib.h"
#include <string>
#include <vector>

// Breaks text into lines that fit within maxWidth at the given font size,
// wrapping on spaces. Used for the HELP panel's description paragraph.
static std::vector<std::string> WrapMenuText(Font font, const std::string& text, float maxWidth, float fontSize, float spacing)
{
    std::vector<std::string> lines;
    std::string word, line;

    for (size_t i = 0; i <= text.size(); ++i)
    {
        bool atEnd = (i == text.size());
        char ch = atEnd ? ' ' : text[i];

        if (ch == ' ')
        {
            if (!word.empty())
            {
                std::string candidate = line.empty() ? word : (line + " " + word);
                Vector2 sz = MeasureTextEx(font, candidate.c_str(), fontSize, spacing);
                if (sz.x > maxWidth && !line.empty())
                {
                    lines.push_back(line);
                    line = word;
                }
                else
                {
                    line = candidate;
                }
                word.clear();
            }
        }
        else
        {
            word += ch;
        }
    }

    if (!line.empty()) lines.push_back(line);
    return lines;
}

MenuResult Menu::show()
{
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Unmatched - Main Menu");
    SetTargetFPS(60);

    Texture2D bg = LoadTexture("assets/menu_bg.png");
    if (bg.id != 0)
    {
        SetTextureFilter(bg, TEXTURE_FILTER_BILINEAR);
    }

    Font titleFont = LoadFontEx("assets/fonts/Cinzel-Black.ttf", 110, nullptr, 0);
    Font semiFont = LoadFontEx("assets/fonts/Cinzel-SemiBold.ttf", 56, nullptr, 0);
    Font regularFont = LoadFontEx("assets/fonts/Cinzel-Regular.ttf", 56, nullptr, 0);

    if (titleFont.texture.id != 0) SetTextureFilter(titleFont.texture, TEXTURE_FILTER_BILINEAR);
    if (semiFont.texture.id != 0) SetTextureFilter(semiFont.texture, TEXTURE_FILTER_BILINEAR);
    if (regularFont.texture.id != 0) SetTextureFilter(regularFont.texture, TEXTURE_FILTER_BILINEAR);

    Rectangle startBtn = { (float)screenWidth / 2 - 210, 420, 420, 64 };
    Rectangle loadBtn  = { (float)screenWidth / 2 - 210, 505, 420, 56 };
    Rectangle helpBtn  = { (float)screenWidth / 2 - 210, 580, 420, 56 };
    Rectangle exitBtn  = { (float)screenWidth / 2 - 210, 655, 420, 56 };

    Color gold = { 212, 175, 55, 255 };
    Color glass = { 20, 24, 38, 190 };
    Color glassHover = { 32, 38, 58, 220 };
    Color redBtn = { 135, 12, 18, 255 };
    Color redHover = { 165, 20, 28, 255 };

    bool showHelp = false;
    bool helpJustOpened = false;
    float helpScroll = 0.0f;

    const std::string helpTitle = "HOW TO PLAY";
    const std::string helpBody =
        "Unmatched: Gothic Shadows pits two legendary heroes against each "
        "other on the fog-shrouded grounds of Baskerville Manor -- choose "
        "Dracula, Sherlock Holmes, or the unseen Invisible Man. Each round "
        "you play cards from your hand to maneuver across the map, scheme "
        "for an edge, and attack your rival. Every card serves double duty "
        "as a movement or scheme effect and as an attack or defense value, "
        "so managing your hand wisely is the key to victory. Sidekicks "
        "such as Watson and the Sisters can support their hero, block "
        "enemy movement, and soak up damage. Reduce your opponent's hero "
        "to 0 health before they do the same to you, and the shadows of "
        "Baskerville Manor will be yours.";

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        bool hoverStart = !showHelp && CheckCollisionPointRec(mouse, startBtn);
        bool hoverLoad  = !showHelp && CheckCollisionPointRec(mouse, loadBtn);
        bool hoverExit  = !showHelp && CheckCollisionPointRec(mouse, exitBtn);
        bool hoverHelp  = !showHelp && CheckCollisionPointRec(mouse, helpBtn);

        if (hoverStart && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            UnloadFont(titleFont);
            UnloadFont(semiFont);
            UnloadFont(regularFont);
            UnloadTexture(bg);
            CloseWindow();
            return MenuResult::Start;
        }

        if (hoverLoad && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            UnloadFont(titleFont);
            UnloadFont(semiFont);
            UnloadFont(regularFont);
            UnloadTexture(bg);
            CloseWindow();
            return MenuResult::Load;
        }

        if (hoverExit && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            UnloadFont(titleFont);
            UnloadFont(semiFont);
            UnloadFont(regularFont);
            UnloadTexture(bg);
            CloseWindow();
            return MenuResult::Exit;
        }

        if (hoverHelp && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            showHelp = true;
            helpJustOpened = true;
            helpScroll = 0.0f;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (bg.id != 0)
        {
            DrawTexturePro(
                bg,
                { 0, 0, (float)bg.width, (float)bg.height },
                { 0, 0, (float)screenWidth, (float)screenHeight },
                { 0, 0 },
                0,
                WHITE
            );
        }

        DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 140 });

        Vector2 subTextSize = MeasureTextEx(regularFont,
                                            "THE ETERNAL BATTLE OF SHADOWS",
                                            20, 1.2f);

        DrawTextEx(regularFont,
                   "THE ETERNAL BATTLE OF SHADOWS",
                   { (float)screenWidth / 2 - subTextSize.x / 2.0f, 250 },
                   20, 1.2f,
                   { 180, 140, 70, 255 });

        Vector2 titleSize = MeasureTextEx(titleFont, "UNMATCHED", 76, 1.0f);

        DrawTextEx(titleFont,
                   "UNMATCHED",
                   { (float)screenWidth / 2 - titleSize.x / 2.0f, 285 },
                   76, 1.0f,
                   gold);

        // ================= START =================
        DrawRectangleRounded(startBtn, 0.22f, 20,
                             hoverStart ? redHover : redBtn);

        DrawRectangleRoundedLines(startBtn, 0.22f, 20, 1.2f,
                                    { 255, 255, 255, 35 });

        Vector2 startTextSize = MeasureTextEx(semiFont, "START", 22, 1.0f);

        DrawTextEx(semiFont,
                   "START",
                   {
                       startBtn.x + (startBtn.width - startTextSize.x) / 2.0f,
                       startBtn.y + 17
                   },
                   22, 1.0f,
                   WHITE);

        // ================= LOAD GAME =================
        DrawRectangleRounded(loadBtn, 0.22f, 20,
                             hoverLoad ? glassHover : glass);

        DrawRectangleRoundedLines(loadBtn, 0.22f, 20, 1.2f,
                                    { 255, 255, 255, 35 });

        DrawText("[ SAVE ]",
                 (int)(loadBtn.x + 30),
                 (int)(loadBtn.y + 18),
                 16,
                 gold);

        Vector2 loadTextSize = MeasureTextEx(regularFont,
                                             "LOAD GAME",
                                             19, 0.8f);

        DrawTextEx(regularFont,
                   "LOAD GAME",
                   {
                       loadBtn.x + (loadBtn.width / 2.0f) - (loadTextSize.x / 2.0f) + 12,
                       loadBtn.y + 16
                   },
                   19, 0.8f,
                   { 230, 230, 235, 255 });

        // ================= HELP =================
        DrawRectangleRounded(helpBtn, 0.22f, 20,
                             hoverHelp ? glassHover : glass);

        DrawRectangleRoundedLines(helpBtn, 0.22f, 20, 1.2f,
                                    { 255, 255, 255, 35 });

        Vector2 helpTextSize = MeasureTextEx(regularFont,
                                             "HELP",
                                             19, 0.8f);

        DrawTextEx(regularFont,
                   "HELP",
                   {
                       helpBtn.x + (helpBtn.width - helpTextSize.x) / 2.0f,
                       helpBtn.y + 16
                   },
                   19, 0.8f,
                   { 230, 230, 235, 255 });

        // ================= EXIT =================
        DrawRectangleRounded(exitBtn, 0.22f, 20,
                             hoverExit ? glassHover : glass);

        DrawRectangleRoundedLines(exitBtn, 0.22f, 20, 1.2f,
                                    { 255, 255, 255, 35 });

        Vector2 exitTextSize = MeasureTextEx(regularFont,
                                             "EXIT",
                                             19, 0.8f);

        DrawTextEx(regularFont,
                   "EXIT",
                   {
                       exitBtn.x + (exitBtn.width - exitTextSize.x) / 2.0f,
                       exitBtn.y + 16
                   },
                   19, 0.8f,
                   { 230, 230, 235, 255 });

        // ================= HELP OVERLAY =================
        if (showHelp)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 195 });

            Rectangle panel = { (float)screenWidth / 2 - 480, 130, 960, 640 };

            // Soft drop shadow behind the panel for a bit of depth.
            Rectangle shadow = { panel.x + 10, panel.y + 14, panel.width, panel.height };
            DrawRectangleRounded(shadow, 0.05f, 24, { 0, 0, 0, 120 });

            DrawRectangleRounded(panel, 0.05f, 24, { 14, 12, 20, 250 });
            DrawRectangleRoundedLines(panel, 0.05f, 24, 2.0f, gold);
            DrawRectangleRoundedLines(
                { panel.x + 8, panel.y + 8, panel.width - 16, panel.height - 16 },
                0.05f, 24, 1.0f, { 212, 175, 55, 70 }
            );

            // Modal-style close (X) button, top-right corner of the panel.
            Rectangle closeBtn = { panel.x + panel.width - 54, panel.y + 14, 40, 40 };
            bool hoverClose = CheckCollisionPointRec(mouse, closeBtn);

            DrawRectangleRounded(closeBtn, 0.3f, 12, hoverClose ? glassHover : glass);
            DrawRectangleRoundedLines(closeBtn, 0.3f, 12, 1.2f, { 255, 255, 255, 45 });

            Color xColor = hoverClose ? Color{ 235, 235, 235, 255 } : Color{ 190, 185, 178, 255 };
            float xPad = 12.0f;
            DrawLineEx(
                { closeBtn.x + xPad, closeBtn.y + xPad },
                { closeBtn.x + closeBtn.width - xPad, closeBtn.y + closeBtn.height - xPad },
                2.5f, xColor
            );
            DrawLineEx(
                { closeBtn.x + closeBtn.width - xPad, closeBtn.y + xPad },
                { closeBtn.x + xPad, closeBtn.y + closeBtn.height - xPad },
                2.5f, xColor
            );

            Vector2 helpTitleSize = MeasureTextEx(titleFont, helpTitle.c_str(), 42, 1.0f);
            DrawTextEx(titleFont, helpTitle.c_str(),
                       { panel.x + panel.width / 2.0f - helpTitleSize.x / 2.0f, panel.y + 34 },
                       42, 1.0f, gold);

            const char* subtitle = "UNMATCHED: GOTHIC SHADOWS";
            Vector2 subSize = MeasureTextEx(regularFont, subtitle, 15, 1.4f);
            DrawTextEx(regularFont, subtitle,
                       { panel.x + panel.width / 2.0f - subSize.x / 2.0f, panel.y + 90 },
                       15, 1.4f, { 180, 140, 70, 255 });

            DrawLine((int)(panel.x + 70), (int)(panel.y + 122),
                     (int)(panel.x + panel.width - 70), (int)(panel.y + 122),
                     { 212, 175, 55, 90 });

            // Body text area: bigger, more readable font, clipped and
            // scrollable so a larger size can never spill past the panel
            // or collide with the BACK button below it.
            Rectangle backBtn = { panel.x + panel.width / 2.0f - 130, panel.y + panel.height - 90, 260, 56 };

            const float bodyFontSize = 25.0f;
            const float bodySpacing = 0.5f;
            const float bodyLineHeight = 38.0f;
            const float scrollbarW = 5.0f;

            Rectangle textArea = {
                panel.x + 70,
                panel.y + 145,
                panel.width - 140,
                backBtn.y - (panel.y + 145) - 16
            };

            auto bodyLines = WrapMenuText(regularFont, helpBody, textArea.width - scrollbarW - 6.0f, bodyFontSize, bodySpacing);

            float totalTextHeight = bodyLines.size() * bodyLineHeight;
            float maxScroll = totalTextHeight - textArea.height;
            if (maxScroll < 0.0f) maxScroll = 0.0f;

            if (CheckCollisionPointRec(mouse, textArea))
            {
                float wheel = GetMouseWheelMove();
                if (wheel != 0.0f) helpScroll -= wheel * 30.0f;
            }
            if (helpScroll < 0.0f) helpScroll = 0.0f;
            if (helpScroll > maxScroll) helpScroll = maxScroll;

            BeginScissorMode((int)textArea.x, (int)textArea.y, (int)textArea.width, (int)textArea.height);
            float lineY = textArea.y - helpScroll;
            for (const auto& line : bodyLines)
            {
                if (lineY + bodyLineHeight >= textArea.y && lineY <= textArea.y + textArea.height)
                {
                    Vector2 lineSize = MeasureTextEx(regularFont, line.c_str(), bodyFontSize, bodySpacing);
                    DrawTextEx(regularFont, line.c_str(),
                               { textArea.x + (textArea.width - lineSize.x) / 2.0f, lineY },
                               bodyFontSize, bodySpacing, { 230, 226, 218, 255 });
                }
                lineY += bodyLineHeight;
            }
            EndScissorMode();

            if (maxScroll > 0.0f)
            {
                float trackX = textArea.x + textArea.width - scrollbarW;
                DrawRectangle((int)trackX, (int)textArea.y, (int)scrollbarW, (int)textArea.height, Fade(Color{ 90, 80, 85, 255 }, 0.4f));

                float thumbH = textArea.height * (textArea.height / totalTextHeight);
                if (thumbH < 16.0f) thumbH = 16.0f;
                if (thumbH > textArea.height) thumbH = textArea.height;

                float thumbY = textArea.y + (textArea.height - thumbH) * (helpScroll / maxScroll);
                DrawRectangle((int)trackX, (int)thumbY, (int)scrollbarW, (int)thumbH, gold);
            }

            bool hoverBack = CheckCollisionPointRec(mouse, backBtn);

            DrawRectangleRounded(backBtn, 0.22f, 20, hoverBack ? redHover : redBtn);
            DrawRectangleRoundedLines(backBtn, 0.22f, 20, 1.2f, { 255, 255, 255, 35 });

            Vector2 backTextSize = MeasureTextEx(semiFont, "BACK", 20, 1.0f);
            DrawTextEx(semiFont, "BACK",
                       {
                           backBtn.x + (backBtn.width - backTextSize.x) / 2.0f,
                           backBtn.y + 16
                       },
                       20, 1.0f, WHITE);

            bool wantsClose =
                (hoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ||
                (hoverClose && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ||
                IsKeyPressed(KEY_ESCAPE);

            // Ignore the very click that opened the modal, so it can never
            // be interpreted as also closing it in the same frame.
            if (wantsClose && !helpJustOpened)
            {
                showHelp = false;
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
    return MenuResult::Exit;
}