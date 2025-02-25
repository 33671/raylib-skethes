#include "raylib.h"
#include <time.h>

// Define constants
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define NUM_ROWS 10
#define NUM_COLS 10
#define STEP_SIZE 2.0f

int main1(void) {
    // Initialize the window and set target FPS
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Moving Grid");
    SetTargetFPS(60);

    // Seed the random number generator
    SetRandomSeed(time(NULL));

    // SetConfigFlags()
    // SetTargetFPS(int fps)
    // set windows flag here:
    // SetTargetFPS(30);
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);   

    // Define bounds with a margin from the screen edges
    float x_min = 50.0f;
    float x_max = SCREEN_WIDTH - 50.0f;
    float y_min = 50.0f;
    float y_max = SCREEN_HEIGHT - 50.0f;

    // Calculate spacing between points for uniform placement
    float dx = (x_max - x_min) / (NUM_COLS - 1);
    float dy = (y_max - y_min) / (NUM_ROWS - 1);

    // Initialize the grid with Vector2 points
    Vector2 grid[NUM_ROWS][NUM_COLS];
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
            grid[i][j].x = x_min + j * dx;
            grid[i][j].y = y_min + i * dy;
        }
    }

    // Main loop
    while (!WindowShouldClose()) {
        // Update: Move each point randomly and clamp to bounds
        for (int i = 0; i < NUM_ROWS; i++) {
            for (int j = 0; j < NUM_COLS; j++) {
                // Generate random displacement between -1 and 1, scaled by STEP_SIZE
                float rand_dx = STEP_SIZE * ((float)GetRandomValue(0, 1000) / 500.0f - 1.0f);
                float rand_dy = STEP_SIZE * ((float)GetRandomValue(0, 1000) / 500.0f - 1.0f);

                // Apply displacement
                grid[i][j].x += rand_dx;
                grid[i][j].y += rand_dy;

                // Clamp positions to stay within bounds
                if (grid[i][j].x < x_min) grid[i][j].x = x_min;
                else if (grid[i][j].x > x_max) grid[i][j].x = x_max;
                if (grid[i][j].y < y_min) grid[i][j].y = y_min;
                else if (grid[i][j].y > y_max) grid[i][j].y = y_max;
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE); // Clear screen with a white background

        // Draw horizontal lines
        for (int i = 0; i < NUM_ROWS; i++) {
            for (int j = 0; j < NUM_COLS - 1; j++) {
                DrawLineV(grid[i][j], grid[i][j + 1], BLACK);
            }
        }

        // Draw vertical lines
        for (int j = 0; j < NUM_COLS; j++) {
            for (int i = 0; i < NUM_ROWS - 1; i++) {
                DrawLineV(grid[i][j], grid[i + 1][j], BLACK);
            }
        }

        EndDrawing();
    }

    // Clean up
    CloseWindow();
    return 0;
}