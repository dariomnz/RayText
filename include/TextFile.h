#ifndef TextFile_H
#define TextFile_H

#include "Common.h"

typedef struct Line
{
    char *data;      // Data of line
    size_t size;     // Size of data
    size_t str_size; // Size of line
} Line;

typedef struct Cursor
{
    Line *line;      // Pointer to line
    size_t line_num; // Numero de la linea
    size_t position; // Position in line
} Cursor;

typedef struct TextFile
{
    size_t n_lines; // Size of lines
    Line **lines;   // Array of pointers to lines
    Cursor cursor;  // Cursor
    char name[256];
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