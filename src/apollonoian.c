
#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h> // For size_t
#define MAX_CIRCLES 50000
#define EPSILON1 1.0f
#define PI 3.14159265358979323846f

typedef struct {
    float real;
    float imag;
} Complex;

typedef struct {
    Vector2 center;
    float bend;
    float radius;
    Color color;
} Circle;

typedef struct {
    Circle c1;
    Circle c2;
    Circle c3;
} Triplet;

// Complex number operations
Complex complex_add(Complex a, Complex b) {
    return (Complex){a.real + b.real, a.imag + b.imag};
}

Complex complex_sub(Complex a, Complex b) {
    return (Complex){a.real - b.real, a.imag - b.imag};
}

Complex complex_mul(Complex a, Complex b) {
    return (Complex){
        a.real * b.real - a.imag * b.imag,
        a.real * b.imag + a.imag * b.real
    };
}

Complex complex_scale(Complex a, float s) {
    return (Complex){a.real * s, a.imag * s};
}

Complex complex_div(Complex a, float s) {
    return (Complex){a.real / s, a.imag / s};
}

float complex_abs(Complex a) {
    return sqrtf(a.real * a.real + a.imag * a.imag);
}

Complex complex_conj(Complex a) {
    return (Complex){a.real, -a.imag};
}

Complex complex_sqrt(Complex a) {
    float r = sqrtf(complex_abs(a));
    float theta = atan2f(a.imag, a.real) / 2;
    return (Complex){r * cosf(theta), r * sinf(theta)};
}

// Circle operations
Circle create_circle(Complex center, float bend, Color color) {
    Circle c;
    c.center = (Vector2){center.real, center.imag};
    c.bend = bend;
    c.radius = fabsf(1.0f / bend);
    c.color = color;
    return c;
}

float circle_distance(Circle a, Circle b) {
    return Vector2Distance(a.center, b.center);
}


// 判断两个圆是否相切
bool isTangent(const Circle* c1, const Circle* c2) {
    float d = circle_distance(*c1, *c2);
    float r1 = c1->radius;
    float r2 = c2->radius;
    bool a = fabs(d - (r1 + r2)) < EPSILON1;
    bool b = fabs(d - fabs(r2 - r1)) < EPSILON1;
    return a || b;
}

// 验证新圆是否有效
bool validate(const Circle* c4, const Circle* c1, const Circle* c2, const Circle* c3, const Circle* allCircles, size_t numCircles) {
    // 检查半径是否过小
    if (c4->radius < 2) {
        return false;
    }
    bool s = isTangent(c4, c1);
    // 检查与三个基准圆是否都相切
    if (!isTangent(c4, c1) || !isTangent(c4, c2) || !isTangent(c4, c3)) {
        return false;
    }

    // 检查与现有圆是否冲突
    for (size_t i = 0; i < numCircles; ++i) {
        const Circle other = allCircles[i];
        double d = circle_distance(*c4, other);
        double radiusDiff = fabs(c4->radius - other.radius);
        
        // 如果圆心距离和半径差都极小，视为重复圆
        if (d < EPSILON1 && radiusDiff < EPSILON1) {
            return false;
        }
    }

    return true;
}
// Descartes' Theorem implementation
void descartes(Circle c1, Circle c2, Circle c3, float* k4) {
    float k1 = c1.bend;
    float k2 = c2.bend;
    float k3 = c3.bend;
    
    float sum = k1 + k2 + k3;
    float product = sqrtf(fabsf(k1*k2 + k2*k3 + k1*k3));
    
    k4[0] = sum + 2 * product;
    k4[1] = sum - 2 * product;
}

// Generate new circles
void generate_circles(Circle c1, Circle c2, Circle c3, Circle* new_circles, int* count) {
    Complex z1 = {c1.center.x, c1.center.y};
    Complex z2 = {c2.center.x, c2.center.y};
    Complex z3 = {c3.center.x, c3.center.y};
    
    float k1 = c1.bend;
    float k2 = c2.bend;
    float k3 = c3.bend;
    
    Complex zk1 = complex_scale(z1, k1);
    Complex zk2 = complex_scale(z2, k2);
    Complex zk3 = complex_scale(z3, k3);
    
    Complex sum = complex_add(complex_add(zk1, zk2), zk3);
    
    Complex root = complex_sqrt(complex_add(
        complex_add(complex_mul(zk1, zk2), 
        complex_mul(zk2, zk3)), 
        complex_mul(zk1, zk3)
    ));
    root = complex_scale(root, 2);
    
    float k4[2];
    descartes(c1, c2, c3, k4);
    
    *count = 0;
    for (int i = 0; i < 2; i++) {
        if (fabs(k4[i]) < 1e-6) continue;
        
        Complex center_plus = complex_div(complex_add(sum, root), k4[i]);
        Complex center_minus = complex_div(complex_sub(sum, root), k4[i]);
        
        new_circles[*count] = create_circle(center_plus, k4[i], BLUE);
        (*count)++;
        new_circles[*count] = create_circle(center_minus, k4[i], BLUE);
        (*count)++;
    }
}

// // Validation checks
int is_valid(Circle new_c, Circle* circles, int circle_count) {
    if (new_c.radius < 2) return 0;
    
    for (int i = 0; i < circle_count; i++) {
        float d = circle_distance(new_c, circles[i]);
        float r_sum = new_c.radius + circles[i].radius;
        float r_diff = fabsf(new_c.radius - circles[i].radius);
        
        if (fabsf(d - r_sum) > EPSILON1 && fabsf(d - r_diff) > EPSILON1)
            return 0;
    }
    return 1;
}

int main(void) {
    const int screenWidth = 600;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "Apollonian Gasket");
    SetTargetFPS(60);

    Circle circles[MAX_CIRCLES];
    int circle_count = 0;
    
    // Initialize first three circles
    Circle c1 = create_circle((Complex){screenWidth/2, screenHeight/2}, -1.0f/(screenHeight/2), RED);
    circles[circle_count++] = c1;
    
    Circle c2 = create_circle((Complex){150, 300}, 1.0f/150.0, GREEN);
    circles[circle_count++] = c2;
    
    Circle c3 = create_circle((Complex){450, 300}, 1.0f/150.0, YELLOW);
    circles[circle_count++] = c3;
    
    Triplet queue[1000];
    int queue_count = 1;
    queue[0] = (Triplet){c1, c2, c3};
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw all circles
        for (int i = 0; i < circle_count; i++) {
            Circle c = circles[i];
            DrawCircleLines(c.center.x, c.center.y, c.radius, c.color);
            // Draw
        }
        
        // Generate new circles
        if (circle_count < MAX_CIRCLES - 4) {
            Triplet current = queue[--queue_count];
            
            Circle new_circles[4];
            int new_count;
            generate_circles(current.c1, current.c2, current.c3, new_circles, &new_count);
            
            for (int i = 0; i < new_count; i++) {
                if (validate(&(new_circles[i]),&current.c1, &current.c2, &current.c3,circles,circle_count)) {
                    circles[circle_count++] = new_circles[i];
                    
                    // Add new triplets to queue
                    queue[queue_count++] = (Triplet){current.c1, current.c2, new_circles[i]};
                    queue[queue_count++] = (Triplet){current.c1, current.c3, new_circles[i]};
                    queue[queue_count++] = (Triplet){current.c2, current.c3, new_circles[i]};
                }
            }
        }
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}