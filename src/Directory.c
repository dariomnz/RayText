#include "Directory.h"

extern Editor editor;

void Directory_Draw(char *dir_name)
{
    char array_file[50][256] = {};
    unsigned char array_file_type[50] = {};
    char tmp[256] = {};
    unsigned char tmp_type;
    int i_file = 0, j = 0, k = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir(dir_name);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            // if (dir->d_type == DT_REG)
            // {
            strcpy(array_file[i_file], dir->d_name);
            array_file_type[i_file] = dir->d_type;
            // DEBUG("%s DT: %u %s DT: %u", array_file[i_file], array_file_type[i_file], dir->d_name, dir->d_type);
            i_file++;
            // }
            // if (dir->d_type == DT_DIR)
            // {
            //     strcpy(array_dir[i_dir], dir->d_name);
            //     i_dir++;
            // }
        }
        for (k = 0; k <= i_file; k++) /* Sorting files alphabetically */
            for (j = k + 1; j <= i_file; j++)
            {
                if (strcasecmp(array_file[k], array_file[j]) > 0)
                {
                    strcpy(tmp, array_file[k]);
                    strcpy(array_file[k], array_file[j]);
                    strcpy(array_file[j], tmp);
                    tmp_type = array_file_type[k];
                    array_file_type[k] = array_file_type[j];
                    array_file_type[j] = tmp_type;
                }
            }
        // for (k = 0; k <= i_dir; k++) /* Sorting files alphabetically */
        //     for (j = k + 1; j <= i_dir; j++)
        //     {
        //         if (strcasecmp(array_dir[k], array_dir[j]) > 0)
        //         {
        //             strcpy(tmp, array_dir[k]);
        //             strcpy(array_dir[k], array_dir[j]);
        //             strcpy(array_dir[j], tmp);
        //         }
        //     }
        closedir(d);
        Color color = RED;
        for (int a = 0; a < i_file; a++)
        {
            if (array_file_type[a] == DT_REG)
                color = WHITE;
            else if (array_file_type[a] == DT_DIR)
                color = BLUE;
            DrawTextEx(editor.font, array_file[a], (Vector2){0, editor.font_size * a}, editor.font_size, editor.font_spacing, color);
        }
    }
}