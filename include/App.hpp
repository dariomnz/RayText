#ifndef App_H
#define App_H

#include "Common.hpp"
#include "Editor.hpp"
#include "Command.hpp"
#include "TextFile.hpp"
#include "Directories.hpp"

class App
{
public:
    std::vector<TextFile> textFiles;
    Directories directories;
    Editor editor;
    Command command;
    bool the_end = false;
    App();
    ~App();
    void Logic(void);
    void Draw(void);
};

#endif // App_H