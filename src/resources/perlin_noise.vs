#version 330 core

layout(location = 0) in vec3 vertexPosition;

out vec2 fragCoord;

void main() {
    // 正确传递屏幕坐标到片段着色器
    fragCoord = vertexPosition.xy;
    // 修正坐标系统转换（Raylib使用左上角坐标系）
    gl_Position = vec4(
        (vertexPosition.x/400.0 - 1.0),    // 800px -> [-1,1]
        (1.0 - vertexPosition.y/300.0),   // 600px -> [-1,1]
        0.0, 1.0
    );
}