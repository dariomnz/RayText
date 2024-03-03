#ifndef Structs_H
#define Structs_H

#include <stdio.h>

#include "raylib.h"
#include "DArray.h"

// Structs

typedef struct Cursor
{
    DArray_char *line; // Pointer to line
    size_t line_num;   // Numero de la linea
    size_t position;   // Position in line
} Cursor;

typedef struct TextFile
{
    DArray_char **items; // Darray of pointers to lines
    size_t count;        // Darray count
    size_t capacity;     // Darray capacity

    Cursor cursor_start_select; // Cursor start select
    DArray_char cursor_select;  // Cursor select text
    Cursor cursor;              // Cursor
    DArray_char name;           // Name of file
} TextFile;


typedef struct Directory
{
    DArray_char **items; // DArray items for store names
    size_t count;        // DArray count
    size_t capacity;     // DArray capacity
    DArray_char name;    // Name of dir
    int selected;        // Selected dirent
} Directory;

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
    Directory currentDirectory;
    Font font;
    Camera2D camera;
    float zoom;
    Vector2 cursor_pos;

    KeyboardKey key_pressed;
    char char_pressed;

    Editor_state editor_state;
    bool the_end;
}Editor;

#endif // Structs_H