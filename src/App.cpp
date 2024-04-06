#include "App.hpp"

App::App() : textFiles(1), editor(textFiles, directories), command()
{
    DEBUG_MSG("Construct App");
}

App::~App()
{
    DEBUG_MSG("delete App");
}

void App::Logic(void)
{
    editor.Logic();
    command.Logic(*this);
}

void App::Draw(void)
{
    ClearBackground(BLACK);

    editor.Draw();
    command.Draw(*this);
}