#ifndef Editor_H
#define Editor_H

#include "Structs.hpp"
#include <memory>

class TextFile;
class Editor
{
private:
public:
    int screenWidth;
    int screenHeight;
    Font font;
    Camera2D camera;
    float zoom;

    std::vector<TextFile> &textFiles;
    int currentTextFile_index;
    Directories currentDirectory;

    Vector2 cursor_pos;

    KeyboardKey key_pressed;
    char char_pressed;

    Editor_state editor_state;

    TextFile &GetCurrentTextFile();

    Editor(std::vector<TextFile> &ref);
    ~Editor();
    void Logic();
    void Draw();
};

#endif // Editor_H