#include "App.hpp"

App::App() : editor(), command()
{
    DEBUG_MSG("Init App");
}

App::~App()
{
    DEBUG_MSG("Unload App");
}

void App::Logic(void)
{
    editor.Logic();
    command.Logic(this);
}

void App::Draw(void)
{
    ClearBackground(BLACK);

    editor.Draw();
    command.Draw(this);
}