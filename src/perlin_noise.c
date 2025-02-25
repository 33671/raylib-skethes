#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Dynamic Perlin Noise");
    SetTargetFPS(60);

    // 使用自定义顶点着色器（重要修改）
    Shader shader = LoadShader("resources/perlin_noise.vs", "resources/perlin_noise.fs");
    // Shader shader = LoadShader("perlin.vs", "perlin.fs");
    SetShaderValue(shader, GetShaderLocation(shader, "iResolution"), 
              (float[2]){screenWidth, screenHeight}, SHADER_UNIFORM_VEC2);

    // 获取uniform位置
    int timeLoc = GetShaderLocation(shader, "iTime");
    int resLoc = GetShaderLocation(shader, "iResolution");
    float resolution[2] = { (float)screenWidth, (float)screenHeight };
    SetShaderValue(shader, resLoc, resolution, SHADER_UNIFORM_VEC2);

    float time = 0.0f;

    while (!WindowShouldClose()) {
        time = GetTime();
        SetShaderValue(shader, timeLoc, &time, SHADER_UNIFORM_FLOAT);

        BeginDrawing();
            ClearBackground(BLACK);
            BeginShaderMode(shader);
                DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
            EndShaderMode();
        EndDrawing();
    }

    UnloadShader(shader);
    CloseWindow();

    return 0;
}