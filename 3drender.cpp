#include "raylib.h"
#include <math.h>
#include <cstdio>
#include <iostream>

const int width = 1600;
const int height = 900;

Vector2 transform_into_screenspace(Vector2 v)
{
    int minsize = height < width ? height : width;
    return {v.x * minsize + 0.5f * width, v.y * minsize + 0.5f * height};
}

Vector2 project(Vector3 v)
{
    return {v.x / v.z, v.y / v.z};
}

void traslate(Vector3 &v, Vector3 u)
{
    v.x += u.x;
    v.y += u.y;
    v.z += u.z;
}

void rotate_x(Vector3 &v, float angle)
{
    //1   0    0
    //0  cos -sin
    //0  sin  cos
    float sin_a = sinf(angle);
    float cos_a = cosf(angle);
    float x = v.x, y = v.y, z = v.z;

    v.y = y * cos_a - z * sin_a;
    v.z = y * sin_a + z * cos_a;
}

int main(int argc, char **argv)
{
    InitWindow(width, height, "3D Rendering Experiments");

    int is_running = 1;

    Rectangle point;
    point.height = 20;
    point.width = 20;

    Vector3 pts[] =
        {
            {0.5f,   0.5f,   0.5f},
            {0.5f,   0.5f, - 0.5f},
            {0.5f, - 0.5f,   0.5f},
            {0.5f, - 0.5f, - 0.5f},
            {-0.5f,   0.5f,   0.5f},
            {-0.5f,   0.5f, - 0.5f},
            {-0.5f, - 0.5f,   0.5f},
            {-0.5f, - 0.5f, - 0.5f},
        };

    int faces[][3] = 
    {
        {0, 1, 2},
        {1, 2, 3},
        {2, 3, 4},
        {3, 4, 5},
        {4, 5, 6},
        {5, 6, 7},
    };

    float angle = 0.0f;
    Vector3 traslation = {0.0f, 0.0f, 5.0f};
    Vector3 vertex_a, vertex_b;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        //for (auto vertex : pts)
        //{
        //    vertex.z = vertex.z + traslation;

        //    point.x = transform_into_screenspace(project(vertex)).x - 10;
        //    point.y = transform_into_screenspace(project(vertex)).y - 10;
        //    //std::cout << vertex.x << ", " << vertex.y << "\n";
        //    DrawRectangleRec(point, GREEN);
        //}

        angle += 0.001;
        for (auto face : faces)
        {
            for (int i = 0; i < 3; i++)
            {
                vertex_a = pts[face[i]], angle; 
                vertex_b = pts[face[(i + 1) % 3]];

                rotate_x(vertex_a, angle);
                rotate_x(vertex_b, angle);

                traslate(vertex_a, traslation);
                traslate(vertex_b, traslation);

                DrawLineEx(transform_into_screenspace(project(vertex_a)), transform_into_screenspace(project(vertex_b)), 3, GREEN);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}