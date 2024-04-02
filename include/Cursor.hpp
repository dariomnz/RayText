#ifndef Cursor_H
#define Cursor_H

#include "Structs.hpp"
#include "Editor.hpp"
#include "DArray.hpp"
#include "raymath.h"

DArray_char *Cursor_GetLine(TextFile *textFile, Cursor *cursor);

void Cursor_Logic(Editor *editor);
void Cursor_Draw(Editor *editor);

#endif // Cursor_H