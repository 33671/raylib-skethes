#include "dynamic_array.h"
#include "lsystem.h"
#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void print_sequence(const LSymbol *seq, size_t len) {
  for (size_t i = 0; i < len; i++) {
    printf("%s", lsymbol_to_string(seq[i]));
  }
  printf("\n");
}

typedef struct LSysPoint {
  int w;
  int h;
  float angle;
} LSysPoint;

int main() {
  DynamicArray *da_stack = da_init(2, sizeof(LSysPoint));
  if (!da_stack) {
    printf("Failed to initialize dynamic array\n");
    return 1;
  }

  // 定义Algae L-system的公理
  LSymbol algae_axiom[] = {LSYM_F};

  // 定义Algae L-system的规则
  // FF+[+F-F-F]-[-F+F+F]
  LSymbol rule1_successor[] = {
      LSYM_F,        LSYM_F,     LSYM_PLUS,     LSYM_LBRACKET, LSYM_PLUS,
      LSYM_F,        LSYM_MINUS, LSYM_F,        LSYM_MINUS,    LSYM_F,
      LSYM_RBRACKET, LSYM_MINUS, LSYM_LBRACKET, LSYM_MINUS,    LSYM_F,
      LSYM_PLUS,     LSYM_F,     LSYM_PLUS,     LSYM_F,        LSYM_RBRACKET};

  ProductionRule algae_rules[] = {
      {LSYM_F, rule1_successor, sizeof(rule1_successor) / sizeof(LSymbol)},
  };
  // 创建L-system系统
  LSystem *algae =
      lsystem_create(algae_axiom, sizeof(algae_axiom) / sizeof(LSymbol),
                     algae_rules, sizeof(algae_rules) / sizeof(ProductionRule));

  if (!algae) {
    fprintf(stderr, "Failed to create L-system\n");
    return 1;
  }
  int current_length = 200;
  for (int j = 0; j < 5; j++) {
    lsystem_iterate(algae);
    current_length = current_length * 0.5;
  }

  const int screenWidth = 800;
  const int screenHeight = 800;
  InitWindow(screenWidth, screenHeight, "L-System Tree");
  SetTargetFPS(60);
  SetWindowPosition(0, 0);
  float time = 0.0f;
  float swingSpeed = 1.0f;  // 摆动速度
  float swingAmount = 1.0f; // 摆动幅度(度)

  while (!WindowShouldClose()) {
    // int current_length = 20;
    time += GetFrameTime();
    float swingOffset = sinf(time * swingSpeed) * swingAmount;
    BeginDrawing();
    ClearBackground(RAYWHITE);

    float initial_angle = -90.0f; // 初始角度向右
    size_t len = 0;
    int current_w = screenWidth / 2;
    int current_h = screenHeight - 50; // 从底部开始绘制
    float theta = initial_angle;       // 每次迭代重置角度

    const LSymbol *seq = lsystem_get_current(algae, &len);
    bool draw_stoped = false;

    if (draw_stoped)
      break;
    for (int i = 0; i < len; i++) {
      if (seq[i] == LSYM_F) {
        if (current_length < 1) {
          draw_stoped = true;
          break;
        }
        int w_end = current_w + current_length * cosf(theta * DEG2RAD);
        int h_end = current_h + current_length * sinf(theta * DEG2RAD);

        DrawLine(current_w, current_h, w_end, h_end, BLACK);

        current_w = w_end;
        current_h = h_end;
      } else if (seq[i] == LSYM_PLUS) {
        theta += 25; // 向右转
      } else if (seq[i] == LSYM_MINUS) {
        theta -= 25; // 向左转
      } else if (seq[i] == LSYM_LBRACKET) {
        LSysPoint point = {.w = current_w, .h = current_h, .angle = theta};
        if (!da_push(da_stack, &point)) {
          TraceLog(LOG_ERROR, "stack full");
          exit(0);
        }
        // current_length = current_length * 0.5;
      } else if (seq[i] == LSYM_RBRACKET) {
        LSysPoint point = {0};
        if (!da_pop(da_stack, &point)) {
          TraceLog(LOG_WARNING, "probably bad syntax");
          continue;
        }
        current_h = point.h;
        current_w = point.w;
        theta = point.angle;
        // current_length = current_length * 2;
      }
    }
    // lsystem_iterate(algae);
    // current_length = (float)current_length * 0.5;
    // SwapScreenBuffer();
    DrawText("L-System Tree", 10, 10, 20, DARKGRAY);

    EndDrawing();
  }

  CloseWindow();
  da_free(da_stack);
  lsystem_free(algae);
  return 0;
}