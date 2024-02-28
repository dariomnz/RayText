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

void Directory_Load(DArray_char *dir_name);

void Directory_Logic(void);
void Directory_Draw(void);
void Directory_Free(void);

#endif // Directory_H