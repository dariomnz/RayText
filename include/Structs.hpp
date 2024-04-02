#ifndef Structs_H
#define Structs_H

#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#include <string>
#include <vector>

// Structs

typedef struct Cursor
{
    int line_num;    // Numero de la linea
    size_t position; // Position in line
} Cursor;

typedef struct TextFile
{
    std::vector<std::string> buffer; // vector of strings for store the data;

    Cursor cursor_start_select;         // Cursor start select
    std::string cursor_select;          // Cursor select text
    std::vector<Rectangle> rect_select; // Vector of rects
    Cursor cursor;                      // Cursor
    std::string name;                   // Name of file
} TextFile;

typedef struct Directory
{
    std::string name; // Name
    bool is_file;     // Selected dirent
} Directory;

typedef struct Directories
{
    std::vector<Directory> items; // Vector of items for store Directory
    std::string name;             // Name of dir
    int selected;                 // Selected Directory
} Directories;

typedef enum
{
    STATE_TEXTFILE = 0,
    STATE_COMMAND = 1,
    STATE_DIRECTORY = 2
} Editor_state;

typedef struct Editor
{
    bool initialized;
    int screenWidth;
    int screenHeight;
    TextFile currentTextFile;
    std::string currentCommand;
    Directories currentDirectory;
    Font font;
    Camera2D camera;
    float zoom;
    Vector2 cursor_pos;

    KeyboardKey key_pressed;
    char char_pressed;

    Editor_state editor_state;
    bool the_end;
} Editor;

#endif // Structs_H