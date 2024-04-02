#ifndef Cursor_H
#define Cursor_H

#include "Structs.hpp"
#include "Editor.hpp"
#include "raymath.h"
#include <string>

std::string &Cursor_GetLine(TextFile *textFile, Cursor *cursor);

void Cursor_Logic(Editor *editor);
void Cursor_Draw(Editor *editor);

#endif // Cursor_H