#ifndef Structs_H
#define Structs_H

#include <stdio.h>

#include "raylib.h"
#include "DArray.h"

// Structs

typedef struct Cursor
{
    int line_num;    // Numero de la linea
    size_t position; // Position in line
} Cursor;

typedef struct DArray_rectangle
{
    Rectangle *items;     // Array of elements can have any type
    size_t count;    // Count of current elements
    size_t capacity; // Capacity of current items in memory
} DArray_rectangle;

typedef struct TextFile
{
    DArray_char **items; // Darray of pointers to lines
    size_t count;        // Darray count
    size_t capacity;     // Darray capacity

    Cursor cursor_start_select;   // Cursor start select
    DArray_char cursor_select;    // Cursor select text
    DArray_rectangle rect_select; // Darray of rects
    Cursor cursor;                // Cursor
    DArray_char name;             // Name of file
} TextFile;

typedef struct Directory
{
    char *items;     // DArray items for store names
    size_t count;    // DArray count
    size_t capacity; // DArray capacity
    bool is_file;    // Selected dirent
} Directory;

typedef struct Directories
{
    Directory **items; // DArray items for store Directory
    size_t count;      // DArray count
    size_t capacity;   // DArray capacity
    DArray_char name;  // Name of dir
    int selected;      // Selected Directory
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
    DArray_char currentCommand;
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