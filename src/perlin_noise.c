#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Dynamic Perlin Noise");
    SetTargetFPS(60);

    // 使用自定义顶点着色器（重要修改）
    // Shader shader = LoadShader("resources/perlin_noise.vs", "resources/perlin_noise.fs");
    Shader shader = LoadShader("resources/ray_march.vs", "resources/ray_march.fs");
    Shader ascii_shader = LoadShader(0, "resources/origin.fs");

    // 获取uniform位置
    int timeLoc = GetShaderLocation(shader, "iTime");
    int resLoc = GetShaderLocation(shader, "iResolution");
    float resolution[2] = { (float)screenWidth, (float)screenHeight };
    SetShaderValue(shader, resLoc, resolution, SHADER_UNIFORM_VEC2);

    float time = 0.0f;
    RenderTexture2D target_texture = LoadRenderTexture(screenWidth, screenHeight);
    while (!WindowShouldClose()) {
        time = GetTime();
        SetShaderValue(shader, timeLoc, &time, SHADER_UNIFORM_FLOAT);

        // BeginDrawing();
        //     ClearBackground(BLACK);
        //     // BeginShaderMode(shader);
        //        DrawRectangleGradientV(0, 0, screenWidth,screenHeight, RED, GREEN);
        //     // EndShaderMode();
        // EndDrawing();
        BeginTextureMode(target_texture);
            ClearBackground(RAYWHITE);

            BeginShaderMode(shader);
                // DrawRectangleGradientV(0, 0, screenWidth,screenHeight, RED, YELLOW);
                DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
            EndShaderMode();
            
        EndTextureMode();
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginShaderMode(ascii_shader);
                // SetShaderValueTexture(ascii_shader, GetShaderLocation(ascii_shader, "inputTexture"), target_texture.texture);
                DrawTextureRec(target_texture.texture, (Rectangle){ 0, 0, (float)target_texture.texture.width, (float)-target_texture.texture.height }, (Vector2){ 0, 0 }, WHITE);
                // DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
            EndShaderMode();
        EndDrawing();
    }

    UnloadShader(shader);
    CloseWindow();

    return 0;
}