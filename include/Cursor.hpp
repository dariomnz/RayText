#ifndef Cursor_H
#define Cursor_H

#include "Common.hpp"

class TextFile;
class Editor;
class Cursor
{
private:
    TextFile &textFile;

public:
    int line_num;    // Numero de la linea
    size_t position; // Position in line
    Cursor(TextFile &textFile);
    ~Cursor();

    Vector2 GetPosition(Font font);
    std::string &GetLine();
    void GetSelected(Font font);
    void CheckClickCursorPosition(Editor &editor);

    void UpdateSelect(Font font);
    void Move(Font font, KeyboardKey key_pressed);

    void Logic(Editor &editor);
    void Draw(Editor &editor);
};

#endif // Cursor_H