#ifndef App_H
#define App_H

#include "Editor.hpp"
#include "Command.hpp"

class App
{
private:

public:
    Editor editor;
    Command command;
    bool the_end = false;
    App();
    ~App();
    void Logic(void);
    void Draw(void);
};

#endif // App_H