#include "raylib.h"

#include "rlgl.h"
#include "raymath.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG(fmt, ...) fprintf(stderr, "[%s:%d] %s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define STR_MAX_LEN 64

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

// Global variables

TextFile currentTextFile;
char currentCommand[STR_MAX_LEN];
int font_size = 20;
int font_spacing = 0;
Font font;

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
            textFile.lines[i]->size = line_size + 1;
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
    fclose(file);

    textFile.cursor.position = 0;
    textFile.cursor.line = textFile.lines[0];
    textFile.cursor.line_num = 0;

    return textFile;
}

void SaveTextFile(TextFile textFile)
{
    DEBUG("Saving: %s", textFile.name);
    FILE *file;
    // file = fopen("./input2.txt", "w");
    file = fopen(textFile.name, "w");
    if (file == NULL)
    {
        DEBUG("Error: opening the file %s", textFile.name);
        return;
    }

    for (size_t i = 0; i < textFile.n_lines; i++)
    {
        fwrite(textFile.lines[i]->data, textFile.lines[i]->str_size, 1, file);
    }

    fclose(file);
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

void InsertCharCommand(char c)
{
    size_t str_len = strlen(currentCommand);
    if (str_len < STR_MAX_LEN)
    {
        currentCommand[str_len] = c;
        currentCommand[str_len + 1] = '\0';
    }
}

void InsertNewLine(TextFile *textFile)
{
    size_t new_line_pos = textFile->cursor.line_num + 1;
    Line **new_lines = (Line **)realloc(textFile->lines, sizeof(Line *) * (textFile->n_lines + 1));
    if (new_lines == NULL)
    {
        DEBUG("Error: realloc of size: %ld", sizeof(Line *) * (textFile->n_lines + 1));
        return;
    }
    textFile->lines = new_lines;

    Line *new_line = (Line *)malloc(sizeof(Line));
    if (new_lines == NULL)
    {
        DEBUG("Error: malloc of size: %ld", sizeof(Line));
        return;
    }
    size_t new_line_size = textFile->cursor.line->size - textFile->cursor.position;
    new_line->data = malloc(new_line_size);
    if (new_lines == NULL)
    {
        DEBUG("Error: malloc of size: %ld", new_line_size);
        return;
    }
    new_line->size = new_line_size;
    // Copy data
    memcpy(new_line->data, &textFile->cursor.line->data[textFile->cursor.position], new_line_size);
    // Move array by 1
    memcpy(&textFile->lines[new_line_pos + 1], &textFile->lines[new_line_pos], (textFile->n_lines - new_line_pos) * sizeof(Line *));

    textFile->lines[new_line_pos] = new_line;

    // Add return of line
    textFile->cursor.line->data[textFile->cursor.position] = '\n';
    textFile->cursor.line->data[textFile->cursor.position + 1] = '\0';
    textFile->cursor.line->str_size = strlen(textFile->cursor.line->data);

    // Update cursor
    textFile->cursor.line_num++;
    textFile->cursor.line = textFile->lines[textFile->cursor.line_num];
    textFile->cursor.line->str_size = strlen(textFile->cursor.line->data);
    textFile->cursor.position = 0;

    textFile->n_lines++;
}

void RemovePreLine(TextFile *textFile)
{
    if (textFile->cursor.line_num == 0)
        return;

    size_t new_line_size = textFile->lines[textFile->cursor.line_num - 1]->size + textFile->lines[textFile->cursor.line_num]->size;

    char *new_line_data = (char *)realloc(textFile->lines[textFile->cursor.line_num - 1]->data, new_line_size);
    if (new_line_data == NULL)
    {
        DEBUG("Error: realloc of size: %ld", new_line_size);
        return;
    }
    textFile->lines[textFile->cursor.line_num - 1]->data = new_line_data;
    memcpy(&new_line_data[textFile->lines[textFile->cursor.line_num - 1]->str_size - 1], textFile->lines[textFile->cursor.line_num]->data, textFile->lines[textFile->cursor.line_num]->str_size);

    free(textFile->lines[textFile->cursor.line_num]->data);
    free(textFile->lines[textFile->cursor.line_num]);

    memcpy(&textFile->lines[textFile->cursor.line_num], &textFile->lines[textFile->cursor.line_num + 1], (textFile->n_lines - textFile->cursor.line_num - 1) * sizeof(Line *));

    textFile->cursor.line_num--;
    textFile->cursor.line = textFile->lines[textFile->cursor.line_num];
    textFile->cursor.position = textFile->cursor.line->str_size - 1;

    textFile->cursor.line->str_size = strlen(textFile->cursor.line->data);
    textFile->cursor.line->size = new_line_size;
    textFile->cursor.line->data[textFile->cursor.line->str_size + 1] = '\0';

    textFile->n_lines--;
    textFile->lines = (Line **)realloc(textFile->lines, sizeof(Line *) * textFile->n_lines);
}

void RemoveChar(TextFile *textFile)
{
    size_t len = textFile->cursor.line->size;
    size_t cursor_pos = textFile->cursor.position;
    if (cursor_pos == 0)
    {
        RemovePreLine(textFile);
        return;
    }

    memmove(textFile->cursor.line->data + cursor_pos - 1, textFile->cursor.line->data + cursor_pos, (len - cursor_pos) * sizeof(char));
    textFile->cursor.position--;
    textFile->cursor.line->str_size--;
}

void MoveCursor(TextFile *textFile, int key_press)
{
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

Vector2 GetCursorPosition(TextFile textFile)
{
    size_t cursor_pos = textFile.cursor.position;
    char cursor_char = textFile.cursor.line->data[cursor_pos];
    textFile.cursor.line->data[cursor_pos] = '\0';
    Vector2 cursor_vec_pos = MeasureTextEx(font, textFile.cursor.line->data, font_size, font_spacing);
    textFile.cursor.line->data[cursor_pos] = cursor_char;
    cursor_vec_pos.y = font_size * textFile.cursor.line_num;
    return cursor_vec_pos;
}

int Vector2EqualsEpsilon(Vector2 p, Vector2 q, float epsilon)
{
    int result = ((fabsf(p.x - q.x)) <= (epsilon)) &&
                 ((fabsf(p.y - q.y)) <= (epsilon));

    return result;
}

void DrawRectangleCenter(int posX, int posY, int width, int height, Color color)
{
    posX -= width / 2;
    posY -= height / 2;
    DrawRectangle(posX, posY, width, height, color);
}

void DrawTextExCenter(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint)
{
    Vector2 text_size = MeasureTextEx(font, text, fontSize, spacing);
    text_size = Vector2Scale(text_size, 0.5f);
    position = Vector2Subtract(position, text_size);
    DrawTextEx(font, text, position, fontSize, spacing, tint);
}

void DrawCommandBox()
{
    int centerX = GetScreenWidth() / 2;
    int centerY = font_size;
    Vector2 command_size = MeasureTextEx(font, currentCommand, font_size, font_spacing);
    int width = GetScreenWidth() * 0.66f;
    int height = command_size.y + 5;
    if (width < command_size.x)
        width = command_size.x + 5;
    DrawRectangleCenter(centerX, centerY, width + 5, height + 5, WHITE);
    DrawRectangleCenter(centerX, centerY, width, height, GRAY);
    DrawTextExCenter(font, currentCommand, (Vector2){centerX, centerY}, font_size, font_spacing, WHITE);
}

void ConsumeCommand()
{
    size_t str_len = strlen(currentCommand);

    if (strlen(currentCommand) == 0)
        return;
    if (currentCommand[0] == '>')
    {
        if (strcmp(currentCommand, ">save") == 0)
        {
            SaveTextFile(currentTextFile);
            DEBUG(">save");
        }
    }

    currentCommand[0] = '\0';
}
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;
    int char_pressed = 0;
    Vector2 cursor_pos = {0};
    Vector2 cursor_pos_aux = {0};
    char str_line_number[10];
    Vector2 str_line_number_size;
    int key_press = 0;
    int keys[6] = {KEY_RIGHT,
                   KEY_LEFT,
                   KEY_UP,
                   KEY_DOWN,
                   KEY_BACKSPACE,
                   KEY_ENTER};
    bool is_commands_showing = false;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera mouse zoom");

    font = LoadFont("./fonts/Monaco.ttf");
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    Camera2D camera = {0};
    camera.zoom = 1.0f;
    // printf("Prueba debug\n");
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    currentTextFile = LoadTextFile("./input.txt");
    // currentTextFile = LoadTextFile("./main.c");
    // PrintTextFile(currentTextFile);

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        if (IsWindowResized())
        {
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();
        }
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
        key_press = 0;
        for (int i = 0; i < 6; i++)
            if (IsKeyPressed(keys[i]) || IsKeyPressedRepeat(keys[i]))
                key_press = keys[i];

        MoveCursor(&currentTextFile, key_press);
        if (key_press == KEY_BACKSPACE)
            RemoveChar(&currentTextFile);

        char_pressed = GetCharPressed();
        // DEBUG("char: %d", char_pressed);
        if (char_pressed != 0)
        {
            if (is_commands_showing)
                InsertCharCommand((char)char_pressed);
            else
                InsertChar(&currentTextFile, (char)char_pressed);
        }
        if (key_press == KEY_ENTER)
            if (is_commands_showing)
                ConsumeCommand();
            else
                InsertNewLine(&currentTextFile);

        cursor_pos = GetCursorPosition(currentTextFile);

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_P))
        {
            is_commands_showing = !is_commands_showing;
            DEBUG("is_commands_showing %d", is_commands_showing);
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
        {
            SaveTextFile(currentTextFile);
        }
        // DEBUG("Command");
        // if (Vector2Equals(cursor_pos, cursor_pos_aux) == 0)
        // {
        // DEBUG("camera.target %f:%f cursor_pos %f:%f", camera.target.x, camera.target.y, cursor_pos.x, cursor_pos.y);
        //     cursor_pos_aux = cursor_pos;
        // }

        // Zoom based on mouse wheel
        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            // Get the world point that is under the mouse
            // Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            // Set the offset to where the mouse is
            // camera.offset = GetMousePosition();

            // Set the target to match, so that the camera maps the world space point
            // under the cursor to the screen space point under the cursor at any zoom
            // camera.target = mouseWorldPos;

            // Zoom increment
            const float zoomIncrement = 0.125f;

            camera.zoom += (wheel * zoomIncrement);
            if (camera.zoom < zoomIncrement)
                camera.zoom = zoomIncrement;
        }

        camera.offset = (Vector2){screenWidth / 2, screenHeight / 2};
        if (Vector2EqualsEpsilon(cursor_pos, camera.target, 0.1f) == 0)
            camera.target = Vector2Lerp(camera.target, cursor_pos, 0.2f);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        DrawFPS(0, 0);
        BeginMode2D(camera);
        int final_num_pos = 0;
        for (size_t i = 0; i < currentTextFile.n_lines; i++)
        {
            // Draw text
            DrawTextEx(font, currentTextFile.lines[i]->data, (Vector2){0, font_size * i}, font_size, font_spacing, WHITE);
            // Draw line number
            sprintf(str_line_number, "%ld ", i + 1);
            str_line_number_size = MeasureTextEx(font, str_line_number, font_size, font_spacing);
            DrawTextEx(font, str_line_number, (Vector2){-str_line_number_size.x, font_size * i}, font_size, font_spacing, GRAY);
            final_num_pos = font_size * (i + 1);
        }
        // Draw line separator numbers
        DrawLineEx((Vector2){-4, 0}, (Vector2){-4, final_num_pos}, 1, GRAY);
        // Draw cursor
        DrawRectangleV(cursor_pos, (Vector2){2, font_size}, WHITE);

        EndMode2D();

        // Draw menu
        if (is_commands_showing)
        {
            DrawCommandBox();
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    FreeTextFile(currentTextFile);
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}