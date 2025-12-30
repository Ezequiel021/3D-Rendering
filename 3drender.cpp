#include <SDL2/SDL.h>
#include <math.h>
#include <cstdio>
#include <iostream>

struct Vector2
{
    float x, y;
    Vector2()
    {
    }

    Vector2(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    Vector2 operator*(float scalar)
    {
        return Vector2(this->x * scalar, this->y * scalar);
    }

    Vector2 operator+(Vector2 v)
    {
        return Vector2(this->x + v.x, this->y + v.y);
    }
};

struct Vector3
{
    float x, y, z;
    Vector3() {}
    Vector3(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

const int width = 1600;
const int height = 900;

Vector2 transform_into_screenspace(Vector2 v)
{
    int minsize = height < width ? height : width;
    return Vector2(v.x * minsize + 0.5 * width, v.y * minsize + 0.5 * height);
}

Vector2 project(Vector3 v)
{
    return Vector2(v.x / v.z, v.y / v.z);
}

int main(int argc, char **argv)
{

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);

    if (!window)
    {
        printf("%s\n", SDL_GetError());
        return 1;
    }

    if (!renderer)
    {
        printf("%s\n", SDL_GetError());
        return 1;
    }

    SDL_RenderSetScale(renderer, 1, 1);

    SDL_SetWindowTitle(window, "3D graph");
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Event event;

    int is_running = 1;

    SDL_FRect point;
    point.h = 20;
    point.w = 20;
    point.x = width / 2;
    point.y = height / 2;

    SDL_RenderFillRectF(renderer, &point);

    SDL_RenderPresent(renderer);

    Vector3 pts[] =
        {
            Vector3(0.5, 0.5, 0.5),
            Vector3(0.5, 0.5, -0.5),
            Vector3(0.5, -0.5, 0.5),
            Vector3(0.5, -0.5, -0.5),
            Vector3(-0.5, 0.5, 0.5),
            Vector3(-0.5, 0.5, -0.5),
            Vector3(-0.5, -0.5, 0.5),
            Vector3(-0.5, -0.5, -0.5),
        };

    float traslation = 0;

    while (is_running)
    {
        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            is_running = 0;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                is_running = 0;
                break;

            default:
                break;
            }
        default:
            break;
        }

        traslation += 0.001;
        for (auto vertex : pts)
        {
            vertex.z = vertex.z + traslation;

            point.x = transform_into_screenspace(project(vertex)).x;
            point.y = transform_into_screenspace(project(vertex)).y;
            //std::cout << vertex.x << ", " << vertex.y << "\n";

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 128, 255, 128, 255);
            SDL_RenderFillRectF(renderer, &point);

            SDL_RenderPresent(renderer);
        }
    }

    SDL_Quit();
    return 0;
}