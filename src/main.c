#include "raylib.h"

#include "rlgl.h"
#include "raymath.h"

#include "Common.h"
#include "TextFile.h"
#include "Editor.h"

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

// Global variables
extern Editor editor;
// extern int *keys_to_check;
// TextFile currentTextFile;
// char currentCommand[STR_MAX_LEN];
// int font_size = 20;
// int font_spacing = 0;
// Font font;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    // int screenWidth = 800;
    // int screenHeight = 450;
    // int char_pressed = 0;
    // Vector2 cursor_pos = {0};
    // Vector2 cursor_pos_aux = {0};
    // char str_line_number[10];
    // Vector2 str_line_number_size;
    // int key_press = 0;
    // bool is_commands_showing = false;

    // InitWindow(screenWidth, screenHeight, "RayText");

    InitializeEditor();
    
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    // currentTextFile = LoadTextFile("./input.txt");
    // currentTextFile = LoadEmptyTextFile();
    // currentTextFile = LoadTextFile("./main.c");
    // PrintTextFile(currentTextFile);

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        LogicEditor();
        //----------------------------------------------------------------------------------
        // DEBUG("cursor x:y %f:%f offset x:y %f:%f target x:y %f:%f",editor.cursor_pos.x,editor.cursor_pos.y,editor.camera.offset.x,editor.camera.offset.y,editor.camera.target.x,editor.camera.target.y);
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        DrawEditor();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    FreeTextFile(editor.currentTextFile);
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}