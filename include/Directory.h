#ifndef Directory_H
#define Directory_H

#include <dirent.h>
#include "Common.h"

#define MAX_DIRECTORIES 50

typedef struct Directory
{
    char name[MAX_PATH];                  // Name of dir
    struct dirent *data[MAX_DIRECTORIES]; // Array of dirent
    int selected;                         // Selected dirent
} Directory;

#include "Editor.h"

void Directory_Load(char *dir_name);

void Directory_Logic();
void Directory_Draw();

#endif // Directory_H