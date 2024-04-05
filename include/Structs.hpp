#ifndef Structs_H
#define Structs_H

#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#include <string>
#include <vector>

// Structs

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

#endif // Structs_H