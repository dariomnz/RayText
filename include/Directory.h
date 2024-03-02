#ifndef Directory_H
#define Directory_H

#include "Common.h"
#include "DArray.h"

typedef struct Directory
{
    DArray_char **items; // DArray items for store names
    size_t count;        // DArray count
    size_t capacity;     // DArray capacity
    DArray_char name;    // Name of dir
    int selected;        // Selected dirent
} Directory;

#include "Editor.h"

void Directory_Load(Directory *directory, DArray_char *dir_name);

void Directory_Logic(Editor *editor);
void Directory_Draw(Editor *editor);
void Directory_Free(Directory *directory);

#endif // Directory_H