#ifndef Command_H
#define Command_H

#include "Common.hpp"
#include <string>

class App;

class Command
{
private:
    std::string command;

public:
    Command();
    ~Command();

    void Consume(App *app);
    void Logic(App *app);
    void Draw(App *app);
};

#endif // Command_H