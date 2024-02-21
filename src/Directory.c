#include "Directory.h"

extern Editor editor;

void Directory_Update(char *dir_name)
{
    if (strlen(editor.currentDirectory.name) == 0)
    {
        char cwd[256];
        getcwd(cwd, sizeof(cwd));
        sprintf(editor.currentDirectory.name, "%s", cwd);
    }

    char aux[MAX_PATH + 300];
    char aux2[MAX_PATH];
    snprintf(aux, MAX_PATH + 300, "%s/%s", editor.currentDirectory.name, dir_name);
    realpath(aux, aux2);
    sprintf(editor.currentDirectory.name, "%s", aux2);
}

void Directory_Load(char *dir_name)
{
    int i = 0, j = 0, k = 0;
    for (int i = 0; i < MAX_DIRECTORIES; i++)
    {
        editor.currentDirectory.data[i] = NULL;
    }

    DIR *dir;
    struct dirent *tmp;
    Directory_Update(dir_name);
    DEBUG("Opening dir: %s", editor.currentDirectory.name);
    dir = opendir(editor.currentDirectory.name);
    if (dir)
    {
        while (i < MAX_DIRECTORIES && (editor.currentDirectory.data[i] = readdir(dir)) != NULL)
        {
            i++;
        }
        closedir(dir);
        editor.currentDirectory.selected = 0;

        for (k = 0; k < i; k++) /* Sorting files alphabetically */
            for (j = k + 1; j < i; j++)
            {
                if (strcasecmp(editor.currentDirectory.data[k]->d_name, editor.currentDirectory.data[j]->d_name) > 0)
                {
                    tmp = editor.currentDirectory.data[k];
                    editor.currentDirectory.data[k] = editor.currentDirectory.data[j];
                    editor.currentDirectory.data[j] = tmp;
                }
            }
    }
    else
    {
        DEBUG("Error: Cannot open dir: %s", editor.currentDirectory.name);
    }
}

Vector2 Directory_GetCursorPosition()
{
    return (Vector2){0, editor.currentDirectory.selected * editor.font_size};
}

void Directory_Logic()
{
    if (editor.editor_state != STATE_DIRECTORY)
        return;

    if (editor.key_pressed == KEY_DOWN)
    {
        if (editor.currentDirectory.data[editor.currentDirectory.selected + 1] != NULL && editor.currentDirectory.selected + 1 < MAX_DIRECTORIES)
        {
            editor.currentDirectory.selected++;
        }
    }
    if (editor.key_pressed == KEY_UP)
    {
        if (editor.currentDirectory.selected != 0)
        {
            editor.currentDirectory.selected--;
        }
    }
    if (editor.key_pressed == KEY_ENTER)
    {
        if (editor.currentDirectory.data[editor.currentDirectory.selected]->d_type == DT_REG)
        {
            TextFile_Free();
            char aux[MAX_PATH + MAX_PATH];
            snprintf(aux, MAX_PATH + MAX_PATH, "%s/%s", editor.currentDirectory.name, editor.currentDirectory.data[editor.currentDirectory.selected]->d_name);
            editor.currentTextFile = TextFile_Load(aux);
            editor.editor_state = STATE_TEXTFILE;
        }
        else if (editor.currentDirectory.data[editor.currentDirectory.selected]->d_type == DT_DIR)
        {
            Directory_Load(editor.currentDirectory.data[editor.currentDirectory.selected]->d_name);
        }
    }

    editor.cursor_pos = Directory_GetCursorPosition();
}

void Directory_Draw()
{

    if (editor.editor_state != STATE_DIRECTORY)
        return;
    Color color = RED;
    for (int a = 0; a < MAX_DIRECTORIES; a++)
    {
        if (editor.currentDirectory.data[a] == NULL)
            break;
        if (editor.currentDirectory.data[a]->d_type == DT_REG)
            color = WHITE;
        else if (editor.currentDirectory.data[a]->d_type == DT_DIR)
            color = BLUE;
        Vector2 pos = (Vector2){0, editor.font_size * a};
        if (a == editor.currentDirectory.selected)
        {
            Vector2 back_size = MeasureTextEx(editor.font, editor.currentDirectory.data[a]->d_name, editor.font_size, editor.font_spacing);
            DrawRectangleV(pos, back_size, DARKGRAY);
        }
        DrawTextEx(editor.font, editor.currentDirectory.data[a]->d_name, pos, editor.font_size, editor.font_spacing, color);
    }
}