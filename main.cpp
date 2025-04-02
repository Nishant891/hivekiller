#include "raylib.h"
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <map>

using namespace std;

// The width and height of the screen
int screenWidth = 1800;
int screenHeight = 1000;

// The position of the center of the spacecraft
int x = 50;
int y = 50;

// The rotation of the spacecraft
float rotation = 1.0f;

struct Bullet
{
    float bullet_x;
    float bullet_y;
    float bullet_speed;
    float direction_x;
    float direction_y;
    bool active;
};

struct Grid
{
    float x;
    float y;
    float width;
    float height;
};

const float CROSSHAIR_DISTANCE = 80.0f; // Distance from spacecraft center
const float ROTATION_SPEED = 1.0f;

int crosshair_x = CROSSHAIR_DISTANCE;
int crosshair_y = CROSSHAIR_DISTANCE;

vector<Bullet> bullets;

map<int, Grid> rectangles;

bool checkCollision(float x, float y){
    for(const auto &rectangle : rectangles){
        if(CheckCollisionCircleRec(Vector2{x, y}, 10, Rectangle{rectangle.second.x, rectangle.second.y, rectangle.second.width, rectangle.second.height})){
            rectangles.erase(rectangle.first);
            return true;
        }
    }
    return false;
}
int main(void)
{
    InitWindow(screenWidth, screenHeight, "First person shooter"); // Initialize window and OpenGL context
    SetTargetFPS(60);

    for(int i = 0; i < 100; i++){
        Grid rectangle;
        rectangle.x = GetRandomValue(screenWidth/2, screenWidth-24);
        rectangle.y = GetRandomValue(screenHeight/2, screenHeight-24);
        rectangle.width = 24;
        rectangle.height = 24;
        cout<<"rectangle: "<<rectangle.x + rectangle.y<<endl;
        rectangles[rectangle.x + rectangle.y] = rectangle;
    }

    std::vector<Texture2D> frames;

    for (int i = 0; i < 8; i++)
    {
        string blast_filename = "assets/frame_000" + std::to_string(i) + ".png";
        Image blast_img = LoadImage(blast_filename.c_str());
        frames.push_back(LoadTextureFromImage(blast_img));
        UnloadImage(blast_img);
    }

    string spacecraft_filename = "assets/spacecraft.png";
    Image spacecraft_img = LoadImage(spacecraft_filename.c_str());
    ImageResize(&spacecraft_img, spacecraft_img.width / 4, spacecraft_img.height / 4);
    Texture2D spacecraft = LoadTextureFromImage(spacecraft_img);
    UnloadImage(spacecraft_img);

    int currentFrame = 0;
    float frameTime = 0.1f;
    float timer = 0.0f;

    while (!WindowShouldClose())
    {
        timer += GetFrameTime();
        if (timer >= frameTime)
        {
            timer = 0.0f;
            currentFrame = (currentFrame + 1) % frames.size();
        }
        if (IsKeyDown(KEY_U) && rotation > -30.0f)
        {
            rotation -= 1.0f;
        }
        if (IsKeyDown(KEY_H) && rotation < 120.0f)
        {
            rotation += 1.0f;
        }

        float radians = rotation * (PI / 180.0f);
        crosshair_x = x + cos(radians) * CROSSHAIR_DISTANCE;
        crosshair_y = y + sin(radians) * CROSSHAIR_DISTANCE;

        if (IsKeyPressed(KEY_SPACE))
        {
            Bullet bullet;
            bullet.bullet_x = crosshair_x;
            bullet.bullet_y = crosshair_y;
            bullet.bullet_speed = 24;
            bullet.direction_x = cos(radians) * bullet.bullet_speed;
            bullet.direction_y = sin(radians) * bullet.bullet_speed;
            bullet.active = true;
            bullets.push_back(bullet);
        }

        for (auto &bullet : bullets)
        {
            if (bullet.active)
            {
                bullet.bullet_x += bullet.direction_x;
                bullet.bullet_y += bullet.direction_y;

                if (checkCollision(bullet.bullet_x, bullet.bullet_y) || bullet.bullet_x < 0 || bullet.bullet_x > screenWidth || bullet.bullet_y < 0 || bullet.bullet_y > screenHeight)
                {
                    bullet.active = false;
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        Rectangle sourceRec = {-25, -25, (float)spacecraft.width, (float)spacecraft.height};
        Rectangle destRec = {(float)x, (float)y, (float)spacecraft.width + 7, (float)spacecraft.height};
        Vector2 origin = {(float)spacecraft.width / 2, (float)spacecraft.height / 2};
        DrawTexturePro(spacecraft, sourceRec, destRec, origin, rotation - 45, WHITE);

        DrawCircle(crosshair_x, crosshair_y, 5, GREEN);

        for (const auto &bullet : bullets)
        {
            if (bullet.active)
            {
                DrawCircle(bullet.bullet_x, bullet.bullet_y, 10, RED);
            }
        }

        for (const auto &rectangle : rectangles)
        {
            DrawRectangle(rectangle.second.x, rectangle.second.y, rectangle.second.width, rectangle.second.height, GREEN);
        }

        EndDrawing();
    }

    for (Texture2D &tex : frames)
        UnloadTexture(tex);

    CloseWindow();
    return 0;
}