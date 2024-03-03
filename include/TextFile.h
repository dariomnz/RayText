#ifndef TextFile_H
#define TextFile_H

#include "Common.h"
#include "Structs.h"
#include "DArray.h"
#include "Editor.h"

TextFile TextFile_Load(const DArray_char *file_name);
TextFile TextFile_LoadEmpty(void);
void TextFile_Free(TextFile *textFile);
void TextFile_Save(TextFile *textFile);

void TextFile_Print(TextFile *textFile);

void TextFile_InsertChar(TextFile *textFile, char c);
void TextFile_RemoveChar(TextFile *textFile);
void TextFile_InsertNewLine(TextFile *textFile);
void TextFile_RemovePreLine(TextFile *textFile);


DArray_char *TextFile_GetLine(TextFile *textFile, Cursor *cursor);
void TextFile_MoveCursor(TextFile *textFile, KeyboardKey key_pressed);
Vector2 TextFile_GetCursorPosition(TextFile *textFile, Font font);

void TextFile_Logic(Editor *editor);
void TextFile_Draw(Editor *editor);

#endif // TextFile_H