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

#endif // Structs_H