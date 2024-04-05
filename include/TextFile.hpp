#ifndef TextFile_H
#define TextFile_H

#include "Common.hpp"
#include "Structs.hpp"
#include "Editor.hpp"
#include "raymath.h"
#include <string>

class TextFile
{
private:
    void RemoveLine_Left();
    void RemoveLine_Right();
    void RemoveChar_Left();
    void RemoveChar_Right();

public:
    std::vector<std::string> buffer; // vector of strings for store the data;

    Cursor cursor_start_select;         // Cursor start select
    std::string cursor_select;          // Cursor select text
    std::vector<Rectangle> rect_select; // Vector of rects
    Cursor cursor;                      // Cursor
    std::string name;                   // Name of file
    TextFile();
    ~TextFile();

    std::string ToString();

    void Load();
    void Load(std::string file_name);
    void Save();
    void Free();

    void Print();

    void InsertChar(char c);
    void InsertStr(const char *str);
    void RemoveChar();
    void InsertNewLine();
    void RemovePreLine();

    void Logic(Editor &editor);
    void Draw(Editor &editor);
    void DrawNoCamera(Editor &editor);
};

#endif // TextFile_H