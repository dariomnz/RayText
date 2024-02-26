#ifndef Directory_H
#define Directory_H

#include "Common.h"

#define MAX_DIRECTORIES 50

typedef struct Directory_name
{
    char *items;     // DArray items for store names
    size_t count;    // DArray count
    size_t capacity; // DArray capacity
} Directory_name;

typedef struct Directory
{
    Directory_name **items; // DArray items for store names
    size_t count;           // DArray count
    size_t capacity;        // DArray capacity
    char name[MAX_PATH];    // Name of dir
    int selected;           // Selected dirent
} Directory;

#include "Editor.h"

void Directory_Load(const char *dir_name);

void Directory_Logic();
void Directory_Draw();
void Directory_Free();

#endif // Directory_H