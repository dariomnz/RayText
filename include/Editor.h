#ifndef Editor_H
#define Editor_H

struct Editor;
typedef struct Editor Editor;
#include "Common.h"
#include "TextFile.h"
#include "Command.h"
#include "Directory.h"

struct Editor
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

    KeyboardKey key_pressed;
    char char_pressed;

    bool the_end;
};

void Editor_Init();

void Editor_Logic();
void Editor_Draw();

void Editor_Close();

#endif // Editor_H