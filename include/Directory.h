#ifndef Directory_H
#define Directory_H

#include "Common.h"
#include "DArray.h"
#include "Structs.h"
#include "Editor.h"

void Directory_Load(Directory *directory, DArray_char *dir_name);

void Directory_Logic(Editor *editor);
void Directory_Draw(Editor *editor);
void Directory_Free(Directory *directory);

#endif // Directory_H