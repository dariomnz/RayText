#ifndef Editor_H
#define Editor_H

#include "raylib.h"
#include "TextFile.h"

#include <dirent.h>

typedef struct Editor
{
    bool initialized;
    int screenWidth;
    int screenHeight;
    TextFile currentTextFile;
    char currentCommand[STR_MAX_LEN];
    bool is_commands_showing;
    Font font;
    int font_size;
    int font_spacing;
    Camera2D camera;
    Vector2 cursor_pos;
} Editor;

void InitializeEditor();

void LogicEditor();
void DrawEditor();

#endif // Editor_H