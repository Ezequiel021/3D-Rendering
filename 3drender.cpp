#include "raylib.h"
#include <math.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

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
    
    std::string file_name = "cube.obj";
    std::fstream file(file_name);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file " << file_name << "\n";
    }

    std::vector<float> vertices;
    std::vector<int> indices;
    std::vector<int> faceOffset = {0};
    std::vector<int> faceSize;

    std::string input_buffer;
    std::istringstream input_buffer_stream(input_buffer);

    float f_buffer;
    int i_buffer;

    while (file >> input_buffer)
    {
        if (input_buffer == "#")
        {
            std::getline(file, input_buffer);
        }

        if (input_buffer == "v")
        {
            std::getline(file, input_buffer);
            input_buffer_stream = std::istringstream(input_buffer);
            while(input_buffer_stream >> f_buffer)
            {
                vertices.push_back(f_buffer);
            }
        }

        if (input_buffer == "f")
        {
            std::getline(file, input_buffer);

            std::cout << input_buffer << "\n";

            int face_vertex_count = 0;
            while (input_buffer_stream >> i_buffer)
            {
                ++face_vertex_count;
                indices.push_back(i_buffer);
            }
            faceSize.push_back(face_vertex_count);
            faceOffset.push_back(faceOffset[faceOffset.size() - 1] + face_vertex_count);
        }
    }

    file.close();

    Vector2 angle = {0.0f, 0.0f};
    Vector2 last_angle = angle;
    Vector3 traslation = {0.0f, 0.0f, 5.0f};
    Vector3 *vertex3d_buffer;
    Vector2 *vertex_buffer;
    Vector2 triangle[4];

    SetTargetFPS(120);

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
        
        for (int i = 0; i < faceSize.size(); i++)
        {
            vertex3d_buffer = new Vector3[faceSize[i]];
            for (int j = 0; j < faceSize[i]; j++)
            {
                vertex3d_buffer[j].x = vertices[indices[faceOffset[i]]];
                vertex3d_buffer[j].y = vertices[indices[faceOffset[i]] + 1];
                vertex3d_buffer[j].z = vertices[indices[faceOffset[i]] + 2];
                
                traslate(vertex3d_buffer[j], traslation);
                rotate_x(vertex3d_buffer[j], angle.x);
                rotate_y(vertex3d_buffer[j], angle.y);
            }

            vertex_buffer = new Vector2[faceSize[i]];
            for (int j = 0; j < faceSize[i]; j++)
            {
                vertex_buffer[j] = transform_into_screenspace(project(vertex3d_buffer[j]));
            }

            DrawTriangleFan(vertex_buffer, faceSize[i], GREEN);
            delete vertex_buffer;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}