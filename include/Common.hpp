#ifndef Common_H
#define Common_H

#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <vector>
#include <string>

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#define FONT_SIZE 20
#define FONT_SPACING 0

#define DEBUG_MSG(fmt) std::cout << "[" << __FILE__ << ":" << __LINE__ << "] " << __func__ << "(): " << fmt << std::endl;
#define DEBUG_ERROR(fmt) std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] " << __func__ << "() ERROR: " << fmt << std::endl;

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