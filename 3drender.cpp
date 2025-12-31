#include "raylib.h"
#include <math.h>
#include <cstdio>
#include <iostream>
#include <string>

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

void rotate_y(Vector3 &v, float angle)
{
    float sin_a = sinf(angle);
    float cos_a = cosf(angle);
    float x = v.x, y = v.y, z = v.z;

    v.x = x * cos_a + z * sin_a;
    v.z = z * cos_a - x * sin_a;
}

void rotate_z(Vector3 &v, float angle)
{
    float sin_a = sinf(angle);
    float cos_a = cosf(angle);
    float x = v.x, y = v.y, z = v.z;

    v.x = x * cos_a - y * sin_a;
    v.y = x * sin_a + y * cos_a;
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
            {-0.5f, - 0.5f, - 0.5f},
            {-0.5f,   0.5f, - 0.5f},
            {0.5f,   0.5f, - 0.5f},
            {0.5f, - 0.5f, - 0.5f},
            {0.5f, - 0.5f,   0.5f},
            {0.5f,   0.5f,   0.5f},
            {-0.5f,   0.5f,   0.5f},
            {-0.5f, - 0.5f,   0.5f},
        };

    int faces[][3] = 
    {
        {0, 1, 2},
        {2, 3, 0},
        {2, 5, 3},
        {5, 4, 3},
        {6, 5, 4},
        {6, 4, 7},
        {1, 6, 7},
        {1, 7, 0},
    };

    Vector2 angle = {0.0f, 0.0f};
    Vector2 last_angle = angle;
    Vector3 traslation = {0.0f, 0.0f, 5.0f};
    Vector3 vertex_buffer[3];
    Vector2 triangle[4];

    RenderTexture2D face_texture;
    face_texture = LoadRenderTexture(128, 128);

    BeginTextureMode(face_texture);
        ClearBackground(GREEN);
    EndTextureMode();

    SetTargetFPS(120);

    std::cout << sizeof(faces) << "\n";

    char label[] = "0";
    char angle_label[] = "x: 0.00, y: 0.00";


    Vector2 last_mouse_position;
    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        // Calculate angles
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            last_mouse_position = GetMousePosition();
            last_angle = angle;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            angle.x = last_angle.x + 0.01 * (GetMousePosition().x - last_mouse_position.x);
            angle.y = last_angle.y + 0.01 * (GetMousePosition().y - last_mouse_position.y);
        }

        sprintf(angle_label, "x: %.2f, y: %.2f", angle.x, angle.y);
        DrawText(angle_label, 10, 10, 20, RAYWHITE);

        if (IsKeyPressed(KEY_R))
        {
            angle = {0.0f, 0.0f};
        }
        //angle_x = atanf(angle_x);
        //angle_y = atanf(angle_y);
        
        for (auto face : faces)
        {
            for (int i = 0; i < 3; i++)
            {
                vertex_buffer[i] = pts[face[i]];
                rotate_x(vertex_buffer[i], angle.y);
                rotate_y(vertex_buffer[i], angle.x);
                traslate(vertex_buffer[i], traslation);
                
                triangle[i] = transform_into_screenspace(project(vertex_buffer[i]));
                label[0] = '0' + i;
                //DrawText(label, triangle[i].x + 5, triangle[i].y + 5, 20, WHITE);

            }
            triangle[3] = triangle[0];

            DrawLineStrip(triangle, 4, WHITE);
            DrawTriangle(triangle[0], triangle[1], triangle[2], GREEN);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}