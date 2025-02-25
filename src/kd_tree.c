#include <raylib.h>
#include <raymath.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <stdlib.h>

#define MAX_POINTS 1000
#define CANVAS_SIZE 600
#define PADDING 20

typedef struct TreeNode {
    Vector2 point;
    int dimension;
    struct TreeNode* left;
    struct TreeNode* right;
    struct TreeNode* parent;
} TreeNode;

typedef struct {
    TreeNode* node;
    Vector2* remaining;
    int remainingCount;
} ConstructionStep;


// Global state
static Vector2 points[MAX_POINTS];
static int pointCount = 0;
static TreeNode* tree = NULL;
static bool showConstruction = false;
static ConstructionStep* steps = NULL;
static int stepCount = 0;
static int currentStep = 0;
static float interpolation = 0.5f;
static float gridSize = 4.0f;
static int framesCounter = 0;

// Function prototypes
TreeNode* buildKDTree(Vector2* points, int count, int depth, TreeNode* parent, bool recordSteps);
void freeTree(TreeNode* node);
void DrawKDTree(TreeNode* node, int xMin, int yMin, int xMax, int yMax);
void DrawConstructionSteps();
void AddRandomPoints(int count);
void GenerateGrid();
void Reset();
void RebuildTree();
int CompareX(const void* a, const void* b);
int CompareY(const void* a, const void* b);

int main() {
    InitWindow(800, 700, "2D KD-Tree Visualization with Raylib");
    SetTargetFPS(60);
    float prevInterpolation = interpolation; // Track previous value

    GenerateGrid();

    while (!WindowShouldClose()) {
        // Check for interpolation changes
        if (interpolation != prevInterpolation) {
            RebuildTree();
            prevInterpolation = interpolation;
        }

        // Handle mouse input
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            if (mouse.x > PADDING && mouse.x < CANVAS_SIZE - PADDING &&
                mouse.y > PADDING && mouse.y < CANVAS_SIZE - PADDING) {
                if (pointCount < MAX_POINTS) {
                    points[pointCount++] = mouse;
                    RebuildTree();
                }
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw canvas area
        DrawRectangleLines(PADDING, PADDING, CANVAS_SIZE - 2*PADDING, CANVAS_SIZE - 2*PADDING, LIGHTGRAY);

        // Draw grid lines
        for (int i = PADDING; i < CANVAS_SIZE - PADDING; i += 50) {
            DrawLine(i, PADDING, i, CANVAS_SIZE - PADDING, Fade(LIGHTGRAY, 0.3f));
            DrawLine(PADDING, i, CANVAS_SIZE - PADDING, i, Fade(LIGHTGRAY, 0.3f));
        }

        // Draw KD-Tree
        if (showConstruction && steps != NULL) {
            DrawConstructionSteps();
        } else if (tree != NULL) {
            DrawKDTree(tree, PADDING, PADDING, CANVAS_SIZE - PADDING, CANVAS_SIZE - PADDING);
        }

        // Draw points
        for (int i = 0; i < pointCount; i++) {
            DrawCircleV(points[i], 4, BLACK);
        }

        // GUI controls
        GuiSliderBar((Rectangle){ 250, 620, 120, 20 }, "Interpolation", TextFormat("%.2f", interpolation), &interpolation, 0.0f, 1.0f);
        GuiSliderBar((Rectangle){ 250, 650, 120, 20 }, "Grid Size", TextFormat("%.0f", gridSize), &gridSize, 2.0f, 10.0f);

        if (GuiButton((Rectangle){ 400, 620, 150, 30 }, "Generate Grid")) GenerateGrid();
        if (GuiButton((Rectangle){ 400, 655, 150, 30 }, "Add Random Points")) AddRandomPoints(5);
        if (GuiButton((Rectangle){ 570, 620, 100, 30 }, "Reset")) Reset();
        if (GuiButton((Rectangle){ 570, 655, 100, 30 }, showConstruction ? "Hide Steps" : "Show Steps")) 
            showConstruction = !showConstruction;

        EndDrawing();

        // Animation for construction steps
        if (showConstruction && steps != NULL) {
            if (framesCounter % 30 == 0 && currentStep < stepCount - 1) {
                currentStep++;
            }
            framesCounter++;
        }
    }

    // Cleanup
    freeTree(tree);
    if (steps != NULL) free(steps);
    CloseWindow();
    return 0;
}

void RebuildTree() {
    freeTree(tree);
    tree = buildKDTree(points, pointCount, 0, NULL, false);
}
TreeNode* buildKDTree(Vector2* points, int count, int depth, TreeNode* parent, bool recordSteps) {
    if (count == 0) return NULL;

    int dimension = depth % 2;

    // Sort points
    qsort(points, count, sizeof(Vector2), dimension == 0 ? CompareX : CompareY);

    // Calculate split index
    int index = (int)((count - 1) * interpolation);
    index = Clamp(index, 0, count - 1);

    // Create node
    TreeNode* node = malloc(sizeof(TreeNode));
    node->point = points[index];
    node->dimension = dimension;
    node->parent = parent;
    node->left = node->right = NULL;

    if (recordSteps) {
        // Record construction step
        ConstructionStep step = {
            .node = node,
            .remaining = malloc(count * sizeof(Vector2)),
            .remainingCount = count
        };
        memcpy(step.remaining, points, count * sizeof(Vector2));
        steps = realloc(steps, ++stepCount * sizeof(ConstructionStep));
        steps[stepCount - 1] = step;
    }

    // Recursively build subtrees
    node->left = buildKDTree(points, index, depth + 1, node, recordSteps);
    node->right = buildKDTree(points + index + 1, count - index - 1, depth + 1, node, recordSteps);

    return node;
}

void DrawKDTree(TreeNode* node, int xMin, int yMin, int xMax, int yMax) {
    if (node == NULL) return;

    Vector2 point = node->point;
    Color lineColor = node->dimension == 0 ? RED : BLUE;

    // Draw partition line
    if (node->dimension == 0) {
        DrawLine(point.x, yMin, point.x, yMax, lineColor);
    } else {
        DrawLine(xMin, point.y, xMax, point.y, lineColor);
    }

    // Draw subtrees
    if (node->left) {
        if (node->dimension == 0) {
            DrawKDTree(node->left, xMin, yMin, point.x, yMax);
        } else {
            DrawKDTree(node->left, xMin, yMin, xMax, point.y);
        }
    }

    if (node->right) {
        if (node->dimension == 0) {
            DrawKDTree(node->right, point.x, yMin, xMax, yMax);
        } else {
            DrawKDTree(node->right, xMin, point.y, xMax, yMax);
        }
    }
}

void DrawConstructionSteps() {
    if (currentStep >= stepCount) return;

    ConstructionStep step = steps[currentStep];
    
    // Draw previous partitions
    for (int i = 0; i <= currentStep; i++) {
        TreeNode* node = steps[i].node;
        Color lineColor = node->dimension == 0 ? RED : BLUE;
        Vector2 point = node->point;

        if (node->dimension == 0) {
            DrawLine(point.x, PADDING, point.x, CANVAS_SIZE - PADDING, lineColor);
        } else {
            DrawLine(PADDING, point.y, CANVAS_SIZE - PADDING, point.y, lineColor);
        }
    }

    // Draw remaining points
    for (int i = 0; i < step.remainingCount; i++) {
        DrawCircleV(step.remaining[i], 3, Fade(GRAY, 0.5f));
    }

    // Highlight current node
    DrawCircleV(step.node->point, 5, RED);
}

// Helper functions...

int CompareX(const void* a, const void* b) {
    const Vector2* va = (const Vector2*)a;
    const Vector2* vb = (const Vector2*)b;
    return (va->x > vb->x) - (va->x < vb->x);
}

int CompareY(const void* a, const void* b) {
    const Vector2* va = (const Vector2*)a;
    const Vector2* vb = (const Vector2*)b;
    return (va->y > vb->y) - (va->y < vb->y);
}

void AddRandomPoints(int count) {
    for (int i = 0; i < count && pointCount < MAX_POINTS; i++) {
        points[pointCount++] = (Vector2){
            PADDING + (float)(rand() % (CANVAS_SIZE - 2 * PADDING)),
            PADDING + (float)(rand() % (CANVAS_SIZE - 2 * PADDING))
        };
    }
    tree = buildKDTree(points, pointCount, 0, NULL, false);
}

void GenerateGrid() {
    pointCount = 0;
    int gridSizeI = (int)gridSize;
    float cellSize = (CANVAS_SIZE - 2 * PADDING) / (float)(gridSizeI - 1);
    
    for (int x = 0; x < gridSizeI; x++) {
        for (int y = 0; y < gridSizeI; y++) {
            points[pointCount++] = (Vector2){
                PADDING + x * cellSize,
                PADDING + y * cellSize
            };
        }
    }
    tree = buildKDTree(points, pointCount, 0, NULL, false);
}

void Reset() {
    pointCount = 0;
    freeTree(tree);
    tree = NULL;
}

void freeTree(TreeNode* node) {
    if (node == NULL) return;
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}