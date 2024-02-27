#ifndef Editor_H
#define Editor_H

struct Editor;
typedef struct Editor Editor;

#include "Common.h"
#include "TextFile.h"
#include "Command.h"
#include "Directory.h"

typedef enum
{
    STATE_TEXTFILE = 0,
    STATE_COMMAND = 1,
    STATE_DIRECTORY = 2
} Editor_state;

struct Editor
{
    bool initialized;
    int screenWidth;
    int screenHeight;
    TextFile currentTextFile;
    DArray_char currentCommand;
    Directory currentDirectory;
    Font font;
    int font_size;
    int font_spacing;
    Camera2D camera;
    float zoom;
    Vector2 cursor_pos;

    KeyboardKey key_pressed;
    char char_pressed;

    Editor_state editor_state;
    bool the_end;
};

void Editor_Init();

void Editor_Logic();
void Editor_Draw();

void Editor_Close();

#endif // Editor_H