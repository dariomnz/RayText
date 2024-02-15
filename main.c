#include "raylib.h"

#include "rlgl.h"
#include "raymath.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG(fmt, ...) fprintf(stderr, "[%s:%d] %s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

typedef struct Line
{
    char *data;      // Data of line
    size_t size;     // Size of data
    size_t str_size; // Size of line
} Line;

typedef struct Cursor
{
    Line *line;      // Pointer to line
    size_t line_num; // Numero de la linea
    size_t position; // Position in line
} Cursor;

typedef struct TextFile
{
    size_t n_lines; // Size of lines
    Line **lines;   // Array of pointers to lines
    Cursor cursor;  // Cursor
    char name[256];
} TextFile;

size_t GetLineNumber(const char *file_name)
{
    FILE *file;
    char ch;
    size_t line_count = 0;

    file = fopen(file_name, "r");
    if (file == NULL)
    {
        DEBUG("Error: opening the file %s\n", file_name);
        exit(1);
    }

    while ((ch = fgetc(file)) != EOF)
        if (ch == '\n')
            line_count++;

    fclose(file);

    return line_count + 1;
}

TextFile LoadTextFile(const char *file_name)
{
    TextFile textFile;
    FILE *file;
    size_t line_number = GetLineNumber(file_name);
    size_t line_len = 0;
    char *line = NULL;
    size_t line_size;

    printf("Lines %ld\n", line_number);

    file = fopen(file_name, "r");
    if (file == NULL)
    {
        DEBUG("Error: opening the file %s", file_name);
        exit(1);
    }

    sprintf(textFile.name, "%s", file_name);

    textFile.n_lines = line_number;
    textFile.lines = (Line **)malloc(sizeof(Line *) * line_number);

    for (size_t i = 0; i < line_number; i++)
    {
        textFile.lines[i] = (Line *)malloc(sizeof(Line));
        line_size = getline(&line, &line_len, file);
        if (line_size != -1)
        {
            textFile.lines[i]->data = malloc(sizeof(char) * line_size + 1);
            memcpy(textFile.lines[i]->data, line, line_size);
            textFile.lines[i]->data[line_size] = '\0';
            textFile.lines[i]->size = line_size;
        }
        else
        {
            textFile.lines[i]->data = malloc(1);
            textFile.lines[i]->data[0] = '\0';
            textFile.lines[i]->size = 1;
        }

        textFile.lines[i]->str_size = strlen(textFile.lines[i]->data);
        // DEBUG("Size: %ld line: %s", textFile.lines[i]->size, textFile.lines[i]->data);
    }
    free(line);

    textFile.cursor.position = 0;
    textFile.cursor.line = textFile.lines[0];
    textFile.cursor.line_num = 0;

    return textFile;
}

void FreeTextFile(TextFile textFile)
{
    for (size_t i = 0; i < textFile.n_lines; i++)
    {
        free(textFile.lines[i]->data);
        free(textFile.lines[i]);
    }

    free(textFile.lines);
}

void PrintTextFile(TextFile textFile)
{
    printf("--------File \"%s\"--------\n", textFile.name);
    for (size_t i = 0; i < textFile.n_lines; i++)
    {
        printf("%s", textFile.lines[i]->data);
        if (textFile.lines[i]->data[0] == '\0')
            printf("\n");
    }
    printf("------End file \"%s\"------\n", textFile.name);
}

void InsertChar(TextFile *textFile, char c)
{
    size_t len = textFile->cursor.line->size;
    size_t lenstr = strlen(textFile->cursor.line->data);
    size_t cursor_pos = textFile->cursor.position;

    if (lenstr + 1 >= len)
    {
        char *new_data = malloc((len + 128) * sizeof(char));
        if (textFile->cursor.line->data == NULL)
        {
            DEBUG("Error: Memory allocation failed\n");
            exit(1);
        }
        strcpy(new_data, textFile->cursor.line->data);
        free(textFile->cursor.line->data);
        textFile->cursor.line->data = new_data;
        len += 64;
        textFile->cursor.line->size = len;
    }

    memmove(textFile->cursor.line->data + cursor_pos + 1, textFile->cursor.line->data + cursor_pos, (len - cursor_pos) * sizeof(char));
    textFile->cursor.line->data[cursor_pos] = c;
    textFile->cursor.position++;
    textFile->cursor.line->str_size++;
}

void RemoveChar(TextFile *textFile)
{
    size_t len = textFile->cursor.line->size;
    size_t cursor_pos = textFile->cursor.position;
    if (cursor_pos == 0)
        return;

    memmove(textFile->cursor.line->data + cursor_pos - 1, textFile->cursor.line->data + cursor_pos, (len - cursor_pos) * sizeof(char));
    textFile->cursor.position--;
    textFile->cursor.line->str_size--;
}
void MoveCursor(TextFile *textFile)
{
    int key_press = 0;
    int keys[4] = {KEY_RIGHT,
                   KEY_LEFT,
                   KEY_UP,
                   KEY_DOWN};
    for (int i = 0; i < 4; i++)
        if (IsKeyPressed(keys[i]) || IsKeyPressedRepeat(keys[i]))
            key_press = keys[i];

    switch (key_press)
    {
    case KEY_RIGHT:

        textFile->cursor.position++;
        if (textFile->cursor.position + 1 > textFile->cursor.line->str_size)
        {
            key_press = KEY_DOWN;
            if (textFile->cursor.line_num != textFile->n_lines - 1)
                textFile->cursor.position = 0;
        }
        // DEBUG("KEY_RIGHT");
        break;
    case KEY_LEFT:
        if (textFile->cursor.position != 0)
            textFile->cursor.position--;
        else
        {
            key_press = KEY_UP;
            if (textFile->cursor.line_num != 0)
                textFile->cursor.position = __SIZE_MAX__ - 1;
        }
        // DEBUG("KEY_LEFT");
        break;
    default:
        break;
    }
    switch (key_press)
    {
    case KEY_UP:
        if (textFile->cursor.line_num != 0)
        {
            textFile->cursor.line_num--;
            textFile->cursor.line = textFile->lines[textFile->cursor.line_num];
        }
        // DEBUG("KEY_UP");
        break;
    case KEY_DOWN:
        if (textFile->cursor.line_num < textFile->n_lines - 1)
        {
            textFile->cursor.line_num++;
            textFile->cursor.line = textFile->lines[textFile->cursor.line_num];
        }
        // DEBUG("KEY_DOWN");
        break;
    default:
        break;
    }
    if (textFile->cursor.position + 1 > textFile->cursor.line->str_size)
        if (textFile->cursor.line->data[textFile->cursor.line->str_size - 1] == '\n')
            textFile->cursor.position = textFile->cursor.line->str_size - 1;
        else
            textFile->cursor.position = textFile->cursor.line->str_size;

    // DEBUG("cursor_pos %ld str_size %ld line %ld char %c", textFile->cursor.position, textFile->cursor.line->str_size, textFile->cursor.line_num, textFile->cursor.line->data[textFile->cursor.line->str_size - 1]);
}

Vector2 GetCursorPosition(TextFile textFile, Font font, int font_size, int font_spacing)
{
    size_t cursor_pos = textFile.cursor.position;
    char cursor_char = textFile.cursor.line->data[cursor_pos];
    textFile.cursor.line->data[cursor_pos] = '\0';
    Vector2 cursor_vec_pos = MeasureTextEx(font, textFile.cursor.line->data, font_size, font_spacing);
    textFile.cursor.line->data[cursor_pos] = cursor_char;
    cursor_vec_pos.y = font_size * textFile.cursor.line_num;
    return cursor_vec_pos;
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    int font_size = 20;
    int font_spacing = 0;
    int char_pressed = 0;
    Vector2 cursor_pos = {0};

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera mouse zoom");
    Font font = LoadFont("./fonts/Monaco.ttf");

    // Camera2D camera = {0};
    // camera.zoom = 1.0f;
    // printf("Prueba debug\n");
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    TextFile textFile = LoadTextFile("./input.txt");
    PrintTextFile(textFile);

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // Translate based on mouse right click
        // if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        // {
        //     Vector2 delta = GetMouseDelta();
        //     delta = Vector2Scale(delta, -1.0f / camera.zoom);

        //     camera.target = Vector2Add(camera.target, delta);
        // }

        // key_press = GetKeyPressed();
        // if (key_press != 0)
        // {
        // }
        MoveCursor(&textFile);
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE))
            RemoveChar(&textFile);

        char_pressed = GetCharPressed();
        // DEBUG("char: %d", char_pressed);
        if (char_pressed != 0)
        {
            InsertChar(&textFile, (char)char_pressed);
        }
        // if (IsKeyPressed(KEY_ENTER) || IsKeyPressedRepeat(KEY_ENTER))
        //     InsertChar(&textFile, '\n');

        cursor_pos = GetCursorPosition(textFile, font, font_size, font_spacing);

        // // Zoom based on mouse wheel
        // float wheel = GetMouseWheelMove();
        // if (wheel != 0)
        // {
        //     // Get the world point that is under the mouse
        //     Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

        //     // Set the offset to where the mouse is
        //     camera.offset = GetMousePosition();

        //     // Set the target to match, so that the camera maps the world space point
        //     // under the cursor to the screen space point under the cursor at any zoom
        //     camera.target = mouseWorldPos;

        //     // Zoom increment
        //     const float zoomIncrement = 0.125f;

        //     camera.zoom += (wheel * zoomIncrement);
        //     if (camera.zoom < zoomIncrement)
        //         camera.zoom = zoomIncrement;
        // }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);
        for (size_t i = 0; i < textFile.n_lines; i++)
        {
            DrawTextEx(font, textFile.lines[i]->data, (Vector2){0, font_size * i}, font_size, font_spacing, WHITE);
        }
        // MeasureTextEx()
        DrawRectangleV(cursor_pos, (Vector2){5, font_size}, WHITE);

        // BeginMode2D(camera);

        // // Draw the 3d grid, rotated 90 degrees and centered around 0,0
        // // just so we have something in the XY plane
        // rlPushMatrix();
        // rlTranslatef(0, 25 * 50, 0);
        // rlRotatef(90, 1, 0, 0);
        // DrawGrid(100, 50);
        // rlPopMatrix();

        // // Draw a reference circle
        // DrawCircle(100, 100, 50, YELLOW);

        // EndMode2D();

        // DrawText("Mouse right button drag to move, mouse wheel to zoom", 10, 10, 20, WHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    FreeTextFile(textFile);
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}