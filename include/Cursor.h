#ifndef Cursor_H
#define Cursor_H

#include "DArray.h"
#include "Structs.h"
#include "Editor.h"

DArray_char *Cursor_GetLine(TextFile *textFile, Cursor *cursor);

void Cursor_Logic(Editor *editor);
void Cursor_Draw(Editor *editor);

#endif // Cursor_H