#ifndef Directory_H
#define Directory_H

#include "Common.h"
#include "DArray.h"

typedef struct Directory
{
    DArray_char **items; // DArray items for store names
    size_t count;        // DArray count
    size_t capacity;     // DArray capacity
    char name[MAX_PATH]; // Name of dir
    int selected;        // Selected dirent
} Directory;

#include "Editor.h"

void Directory_Load(const char *dir_name);

void Directory_Logic();
void Directory_Draw();
void Directory_Free();

#endif // Directory_H