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

// Función para calcular normales
std::vector<Vector3> CalculateFaceNormals(
    const std::vector<float> &vertices,
    const std::vector<int> &indices,
    const std::vector<int> &faceSize,
    const std::vector<int> &faceOffset)
{
    std::vector<Vector3> normals;
    normals.reserve(faceSize.size()); // Reservar memoria para eficiencia

    for (size_t i = 0; i < faceSize.size(); i++)
    {
        // Necesitamos al menos 3 vértices para definir un plano
        if (faceSize[i] < 3)
        {
            normals.push_back({0, 0, 0}); // Normal inválida o por defecto
            continue;
        }

        // 1. Obtener los índices de los primeros 3 vértices de la cara (triángulo base)
        int idx0 = indices[faceOffset[i]];
        int idx1 = indices[faceOffset[i] + 1];
        int idx2 = indices[faceOffset[i] + 2];

        // 2. Recuperar las coordenadas (Recordando que OBJ empieza en 1, restamos 1)
        // Multiplicamos por 3 porque el vector vertices es plano [x,y,z, x,y,z...]
        int p0 = (idx0 - 1) * 3;
        int p1 = (idx1 - 1) * 3;
        int p2 = (idx2 - 1) * 3;

        Vector3 v0 = {vertices[p0], vertices[p0 + 1], vertices[p0 + 2]};
        Vector3 v1 = {vertices[p1], vertices[p1 + 1], vertices[p1 + 2]};
        Vector3 v2 = {vertices[p2], vertices[p2 + 1], vertices[p2 + 2]};

        // 3. Calcular dos vectores sobre la superficie de la cara
        // Vector A = v1 - v0
        Vector3 A = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
        // Vector B = v2 - v0
        Vector3 B = {v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};

        // 4. Producto Cruz (Cross Product): A x B
        // Esto nos da un vector perpendicular a ambos
        Vector3 normal;
        normal.x = A.y * B.z - A.z * B.y;
        normal.y = A.z * B.x - A.x * B.z;
        normal.z = A.x * B.y - A.y * B.x;

        // 5. Normalizar el vector (hacer que su longitud sea 1)
        float length = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        if (length > 0)
        {
            normal.x /= length;
            normal.y /= length;
            normal.z /= length;
        }

        normals.push_back(normal);
    }

    return normals;
}

// ... (Mantén tus funciones transform_into_screenspace, project, traslate, rotate aquí) ...
// Asegúrate de incluir las funciones que ya tenías arriba del main.
Vector3 cross_prod(Vector3 a, Vector3 b)
{
    return {a.y * b.z - a.z * b.y, a.x * b.z - a.z * b.x, a.x * b.y - a.y * b.x};
}

void normalize(Vector3 &v)
{
    float inv_magnitude = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (fabsf(1.0f - inv_magnitude) > 0.0001f)
    {
        v.x *= inv_magnitude;
        v.y *= inv_magnitude;
        v.z *= inv_magnitude;
    }
}

Vector2 transform_into_screenspace(Vector2 v)
{
    int minsize = height < width ? height : width;
    return {
        v.x * minsize + 0.5f * width, 
        -v.y * minsize + 0.5f * height
    };
}

Vector2 project(Vector3 v)
{
    // Protección simple contra división por cero
    if (v.z == 0)
        v.z = 0.001f;
    return {v.x / v.z, v.y / v.z};
}

void traslate(Vector3 &v, Vector3 u)
{
    v.x += u.x;
    v.y += u.y;
    v.z += u.z;
}

Vector2 add(Vector2 a, Vector2 b)
{
    return {a.x + b.x, a.y + b.y};
}

void rotate_x(Vector3 &v, float angle)
{
    float sin_a = sinf(angle);
    float cos_a = cosf(angle);
    float y = v.y, z = v.z;
    v.y = y * cos_a - z * sin_a;
    v.z = y * sin_a + z * cos_a;
}

void rotate_y(Vector3 &v, float angle)
{
    float sin_a = sinf(angle);
    float cos_a = cosf(angle);
    float x = v.x, z = v.z;
    v.x = x * cos_a + z * sin_a;
    v.z = z * cos_a - x * sin_a;
}

float dot_prod(Vector3 u, Vector3 v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

Vector3 scale(Vector3 v, float s)
{
    return {v.x * s, v.y * s, v.z * s};
}
// ... fin funciones auxiliares ...

int main(int argc, char **argv)
{
    InitWindow(width, height, "3D Rendering Experiments");

    // OBJ Loading Logic
    std::string file_name = argv[1];
    std::fstream file(file_name);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file " << file_name << "\n";
        return -1; // Salir si falla
    }

    std::vector<float> vertices;
    std::vector<int> indices;
    std::vector<int> faceOffset = {0};
    std::vector<int> faceSize;

    std::string input_buffer;
    float f_buffer;
    int i_buffer;
    std::string trash; // Para descartar partes del formato como "f 1//1 2//2"

    while (file >> input_buffer)
    {
        if (input_buffer == "#")
        {
            std::getline(file, input_buffer);
        }
        else if (input_buffer == "v")
        {
            std::getline(file, input_buffer);
            std::istringstream input_buffer_stream(input_buffer);
            while (input_buffer_stream >> f_buffer)
            {
                vertices.push_back(f_buffer);
            }
        }
        else if (input_buffer == "f")
        {
            std::getline(file, input_buffer);
            // CORRECCIÓN 1: Actualizar el stream con la nueva línea
            std::istringstream input_buffer_stream(input_buffer);

            int face_vertex_count = 0;

            // Nota: Los OBJ a veces tienen formato v/vt/vn (ej: 1/2/3).
            // Este parser asume solo enteros simples separados por espacios.
            // Si tu cube.obj tiene barras (/), necesitarás un parser más robusto.
            while (input_buffer_stream >> i_buffer)
            {
                ++face_vertex_count;
                indices.push_back(i_buffer);

                // Si hay caracteres extra (como /), intentar limpiarlos
                if (input_buffer_stream.peek() == '/')
                {
                    input_buffer_stream.ignore(256, ' ');
                }
            }

            if (face_vertex_count > 0)
            { // Solo añadir si encontramos vértices
                faceSize.push_back(face_vertex_count);
                faceOffset.push_back(faceOffset.back() + face_vertex_count);
            }
        }
    }
    file.close();

    std::vector<Vector3> normals = CalculateFaceNormals(vertices, indices, faceSize, faceOffset);

    Vector2 angle = {0.0f, 0.0f};
    Vector2 last_angle = angle;
    Vector3 traslation = {0.0f, 0.0f, 5.0f};
    Vector3 light_direction = {0.0f, 0.0f, -1.0f};
    normalize(light_direction);

    // Eliminamos los punteros globales innecesarios para evitar fugas

    SetTargetFPS(60);                  // 120 puede ser mucho si no está optimizado
    Vector2 last_mouse_position = {0}; // Inicializar

    while (!WindowShouldClose())
    {
        // Update
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            last_mouse_position = GetMousePosition();
            last_angle = angle;
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 delta = {GetMousePosition().x - last_mouse_position.x, GetMousePosition().y - last_mouse_position.y};
            angle.x = last_angle.x - 0.01f * delta.x;
            angle.y = last_angle.y + 0.01f * delta.y;
        }
        if (IsKeyPressed(KEY_R))
            angle = {0.0f, 0.0f};

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText(TextFormat("x: %.2f, y: %.2f", angle.x, angle.y), 10, 10, 20, RAYWHITE);

        // CORRECCIÓN 2: Iterar basado en faceSize, no faceOffset
        for (int i = 0; i < faceSize.size(); i++)
        {

            Vector3 normal = scale(normals[i], -1.0f);
            rotate_x(normal, angle.y);
            rotate_y(normal, angle.x);

            if (normal.z > 0)
            {
                int currentFaceSize = faceSize[i];

                // Usamos vector dinámico local (RAII) para evitar new/delete manual y fugas
                std::vector<Vector2> screen_vertices(currentFaceSize);

                for (int j = 0; j < currentFaceSize; j++)
                {
                    // CORRECCIÓN 3: Lógica de índices correcta
                    // 1. Obtener índice del array de indices global
                    // Offset de la cara actual + vértice actual de la cara (j)
                    int index_in_indices = faceOffset[i] + j;

                    // 2. Obtener el índice del vértice en formato OBJ (Base-1)
                    int obj_vertex_index = indices[index_in_indices];

                    // 3. Convertir a índice de array de floats (Base-0, saltos de 3)
                    int float_base_index = (obj_vertex_index - 1) * 3;

                    // Seguridad básica
                    if (float_base_index < 0 || float_base_index + 2 >= vertices.size())
                        continue;

                    Vector3 v3;
                    v3.x = vertices[float_base_index];
                    v3.y = vertices[float_base_index + 1];
                    v3.z = vertices[float_base_index + 2];

                    rotate_x(v3, angle.y); // Usualmente mouse Y rota en eje X
                    rotate_y(v3, angle.x); // Mouse X rota en eje Y
                    traslate(v3, traslation);

                    screen_vertices[j] = transform_into_screenspace(project(v3));
                    DrawLineV(screen_vertices[j], transform_into_screenspace(project(normal)), RED);
                }

                Color face_color = normal.z > 0.0f ? BLUE : GREEN;
                float intensity = normal.z > 0.0f ? normal.z : -normal.z;
                face_color.r *= intensity;
                face_color.g *= intensity;
                face_color.b *= intensity;
                // Raylib espera un puntero, vector.data() nos lo da
                DrawTriangleFan(screen_vertices.data(), currentFaceSize, face_color);

                // Dibuja bordes para ver mejor la forma 3D (opcional)
                // for (int k = 0; k < currentFaceSize; k++)
                // {
                //     DrawLineV(screen_vertices[k], screen_vertices[(k + 1) % currentFaceSize], DARKGREEN);
                // }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}