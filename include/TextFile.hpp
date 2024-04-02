#ifndef TextFile_H
#define TextFile_H

#include "Common.hpp"
#include "Structs.hpp"
#include "Editor.hpp"
#include "raymath.h"
#include <string>

TextFile TextFile_Load(std::string file_name);
TextFile TextFile_LoadEmpty(void);
void TextFile_Free(TextFile *textFile);
void TextFile_Save(TextFile *textFile);

void TextFile_Print(TextFile *textFile);

void TextFile_InsertChar(TextFile *textFile, char c);
void TextFile_RemoveChar(TextFile *textFile);
void TextFile_InsertNewLine(TextFile *textFile);
void TextFile_RemovePreLine(TextFile *textFile);

void TextFile_MoveCursor(TextFile *textFile, KeyboardKey key_pressed);
Vector2 TextFile_GetCursorPosition(TextFile *textFile, Font font);

void TextFile_Logic(Editor *editor);
void TextFile_Draw(Editor *editor);

#endif // TextFile_H