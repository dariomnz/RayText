#include "Common.hpp"

int FloatEqualsEpsilon(float x, float y, float epsilon)
{
    int result = (fabsf(x - y)) <= (epsilon);

    return result;
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