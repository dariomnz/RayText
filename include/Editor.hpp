#ifndef Editor_H
#define Editor_H

#include "Structs.hpp"
#include "Command.hpp"

class Editor
{
private:
public:
    int screenWidth;
    int screenHeight;
    Font font;
    Camera2D camera;
    float zoom;

    TextFile currentTextFile;
    Directories currentDirectory;

    Vector2 cursor_pos;

    KeyboardKey key_pressed;
    char char_pressed;

    Editor_state editor_state;

    Editor();
    ~Editor();
    void Logic();
    void Draw();
};

#endif // Editor_H