// #include "Common.hpp"
#include "App.hpp"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    SetTargetFPS(60);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    App app;

    // Main game loop
    while (!WindowShouldClose() && !app.the_end) // Detect window close button or ESC key
    {
        //----------------------------------------------------------------------------------
        // Logic
        //----------------------------------------------------------------------------------
        app.Logic();
        //----------------------------------------------------------------------------------

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        app.Draw();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    return 0;
}