#pragma once

#include "raylib.h"

void LoadGameFonts();
void UnloadGameFonts();

Font GetTitleFont();    // Cinzel-Black   - big headings / titles
Font GetSemiFont();     // Cinzel-SemiBold - buttons / sub-headings
Font GetRegularFont();  // Cinzel-Regular  - body / labels