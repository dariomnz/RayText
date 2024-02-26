#ifndef Common_H
#define Common_H

#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#define MAX_COMMAND_LEN 64
#define MAX_PATH 1024

#define DEBUG(fmt, ...) fprintf(stderr, "[%s:%d] %s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

/**
 * @param type type of the elemments in the array
 * @param ptr pointer to get the value of each elem in array
 * @param arr the array
 */
#define FOR_EACH(type, ptr, arr) for (type *ptr = arr; ptr < arr + sizeof(arr) / sizeof(arr[0]); ptr++)

int FloatEqualsEpsilon(float x, float y, float epsilon);
int Vector2EqualsEpsilon(Vector2 p, Vector2 q, float epsilon);

void DrawRectangleCenter(int posX, int posY, int width, int height, Color color);
void DrawTextExCenter(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint);

#endif // Common_H