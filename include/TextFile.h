#ifndef TextFile_H
#define TextFile_H

#include "Common.h"

typedef struct Line
{
    char *items;     // Darray items
    size_t count;    // Darray count
    size_t capacity; // Darray capacity
} Line;

typedef struct Cursor
{
    Line *line;      // Pointer to line
    size_t line_num; // Numero de la linea
    size_t position; // Position in line
} Cursor;

typedef struct TextFile
{
    Line **items;    // Darray of pointers to lines
    size_t count;    // Darray count
    size_t capacity; // Darray capacity

    Cursor cursor;  // Cursor
    char name[256]; // Name of file
} TextFile;

#include "Editor.h"

size_t GetLineNumber(const char *file_name);

TextFile TextFile_Load(const char *file_name);
TextFile TextFile_LoadEmpty();
void TextFile_Free();
void TextFile_Save();

void TextFile_Print();

void TextFile_InsertChar();
void TextFile_RemoveChar();
void TextFile_InsertNewLine();
void TextFile_RemovePreLine();

void TextFile_MoveCursor();
Vector2 TextFile_GetCursorPosition();

void TextFile_Logic();
void TextFile_Draw();

#endif // TextFile_H